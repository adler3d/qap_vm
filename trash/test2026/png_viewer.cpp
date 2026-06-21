// png_viewer.cpp
// Компиляция: cl /EHsc /std:c++17 png_viewer.cpp lodepng.cpp user32.lib gdi32.lib

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <map>
#include <thread>
#include <mutex>
#include <atomic>
#include <fstream>
#include <queue>
#include <condition_variable>
#include <memory>
#include <functional>
#include <sstream>

#include "lodepng.h"

namespace fs = std::filesystem;

// Умный указатель для HBITMAP
struct HBITMAPDeleter {
    void operator()(HBITMAP h) const {
        if (h) DeleteObject(h);
    }
};
using UniqueHBITMAP = std::unique_ptr<std::remove_pointer<HBITMAP>::type, HBITMAPDeleter>;

// Умный указатель для HDC
struct HDCDeleter {
    void operator()(HDC h) const {
        if (h) DeleteDC(h);
    }
};
using UniqueHDC = std::unique_ptr<std::remove_pointer<HDC>::type, HDCDeleter>;

// Умный указатель для HBRUSH
struct HBRUSHDeleter {
    void operator()(HBRUSH h) const {
        if (h) DeleteObject(h);
    }
};
using UniqueHBRUSH = std::unique_ptr<std::remove_pointer<HBRUSH>::type, HBRUSHDeleter>;

// Умный указатель для HFONT
struct HFONTDeleter {
    void operator()(HFONT h) const {
        if (h) DeleteObject(h);
    }
};
using UniqueHFONT = std::unique_ptr<std::remove_pointer<HFONT>::type, HFONTDeleter>;

// Режимы отображения
enum ZoomMode {
    ZOOM_FIT,      // Вписать в окно
    ZOOM_100       // 100% (пиксель в пиксель)
};

// Структура для кэшированного изображения
struct CachedImage {
    UniqueHBITMAP hBitmap;
    UniqueHDC hMemDC;
    int width = 0;
    int height = 0;
    std::string filename;
    bool loaded = false;
    bool loading = false;
    
    CachedImage() = default;
    CachedImage(const CachedImage&) = delete;
    CachedImage& operator=(const CachedImage&) = delete;
    
    CachedImage(CachedImage&& other) noexcept
        : hBitmap(std::move(other.hBitmap))
        , hMemDC(std::move(other.hMemDC))
        , width(other.width)
        , height(other.height)
        , filename(std::move(other.filename))
        , loaded(other.loaded)
        , loading(other.loading) {}
    
    CachedImage& operator=(CachedImage&& other) noexcept {
        if (this != &other) {
            hBitmap = std::move(other.hBitmap);
            hMemDC = std::move(other.hMemDC);
            width = other.width;
            height = other.height;
            filename = std::move(other.filename);
            loaded = other.loaded;
            loading = other.loading;
        }
        return *this;
    }
};

// Глобальные переменные
struct ViewerState {
    std::vector<std::string> imageFiles;
    size_t currentIndex = 0;
    
    // Основной кэш (индекс -> изображение)
    std::map<size_t, CachedImage> cache;
    std::mutex cacheMutex;
    
    // Для предзагрузки
    std::atomic<bool> stopPreload{false};
    std::thread preloadThread;
    std::condition_variable preloadCV;
    std::mutex preloadMutex;
    std::queue<size_t> preloadQueue;
    std::atomic<bool> preloadRunning{false};
    
    // Текущее отображаемое изображение
    CachedImage* currentImage = nullptr;
    HWND hWnd = nullptr;
    std::string currentFile;
    
    // Для двойной буферизации
    UniqueHDC backBufferDC;
    UniqueHBITMAP backBufferBitmap;
    int backBufferWidth = 0;
    int backBufferHeight = 0;
    
    // Состояние программы
    ZoomMode zoomMode = ZOOM_FIT;
    bool showInfo = false;          // Показывать ли информацию
    std::string stateFilePath = "png_viewer.state.txt";
    
    // Для статистики
    size_t totalFiles = 0;

    size_t loadCounter = 0;
};

ViewerState g_state;

void EvictOldImages(size_t currentIndex, size_t keepRadius = 8) {
    std::lock_guard<std::mutex> lock(g_state.cacheMutex);
    
    std::vector<size_t> toRemove;
    
    // Находим индексы, которые нужно выгрузить
    for (auto& pair : g_state.cache) {
        size_t index = pair.first;
        
        // Вычисляем расстояние от текущего индекса
        size_t distance = (index > currentIndex) ? 
                          (index - currentIndex) : 
                          (currentIndex - index);
        
        // Если изображение дальше чем keepRadius - выгружаем
        if (distance > keepRadius) {
            toRemove.push_back(index);
        }
    }
    
    // Выгружаем найденные изображения
    for (size_t index : toRemove) {
        auto it = g_state.cache.find(index);
        if (it != g_state.cache.end()) {
            // Освобождаем ресурсы
            if (it->second.hBitmap) {
                it->second.hBitmap.reset();
            }
            if (it->second.hMemDC) {
                it->second.hMemDC.reset();
            }
            it->second.loaded = false;
            it->second.loading = false;
            
            // Удаляем из кэша
            g_state.cache.erase(it);
            
            // Для отладки (можно убрать)
            // OutputDebugStringA(("Evicted: " + std::to_string(index) + "\n").c_str());
        }
    }
}

// Функции для работы с состоянием
std::string GetFileNameFromPath(const std::string& path) {
    fs::path p(path);
    return p.filename().string();
}

void SaveState() {
    if (g_state.currentFile.empty()) return;
    
    try {
        std::ofstream file(g_state.stateFilePath);
        if (file.is_open()) {
            file << g_state.currentFile << "\n";
            file.close();
        }
    } catch (...) {
        // Игнорируем ошибки
    }
}

std::string LoadState() {
    try {
        std::ifstream file(g_state.stateFilePath);
        if (file.is_open()) {
            std::string filename;
            std::getline(file, filename);
            file.close();
            if (!filename.empty()) {
                return filename;
            }
        }
    } catch (...) {
        // Игнорируем ошибки
    }
    return "";
}

// Освобождение кэшированного изображения
void FreeCachedImage(CachedImage& img) {
    img.hBitmap.reset();
    img.hMemDC.reset();
    img.loaded = false;
    img.loading = false;
}

// Создание или изменение размера back buffer
bool EnsureBackBuffer(int width, int height) {
    if (g_state.backBufferDC && 
        g_state.backBufferWidth == width && 
        g_state.backBufferHeight == height) {
        return true;
    }
    
    g_state.backBufferBitmap.reset();
    g_state.backBufferDC.reset();
    
    HDC hdc = GetDC(g_state.hWnd);
    if (!hdc) return false;
    
    g_state.backBufferDC.reset(CreateCompatibleDC(hdc));
    if (!g_state.backBufferDC) {
        ReleaseDC(g_state.hWnd, hdc);
        return false;
    }
    
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    void* bits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(g_state.hWnd, hdc);
    
    if (!hBitmap) return false;
    
    g_state.backBufferBitmap.reset(hBitmap);
    SelectObject(g_state.backBufferDC.get(), g_state.backBufferBitmap.get());
    
    g_state.backBufferWidth = width;
    g_state.backBufferHeight = height;
    
    return true;
}

// Загрузка PNG через LodePNG в HBITMAP
UniqueHBITMAP LoadPNGToBitmap(const std::string& filename, int& outWidth, int& outHeight) {
    std::vector<unsigned char> image;
    unsigned width, height;
    
    unsigned error = lodepng::decode(image, width, height, filename);
    if (error) {
        return nullptr;
    }
    
    outWidth = width;
    outHeight = height;
    
    // LodePNG даёт RGBA, переводим в BGRA для GDI
    for (size_t i = 0; i < image.size(); i += 4) {
        std::swap(image[i], image[i + 2]);
    }
    
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -static_cast<LONG>(height);
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    HDC hdc = GetDC(nullptr);
    void* bits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, hdc);
    
    if (hBitmap && bits) {
        memcpy(bits, image.data(), image.size());
        return UniqueHBITMAP(hBitmap);
    }
    
    if (hBitmap) DeleteObject(hBitmap);
    return nullptr;
}

// Загрузка одного изображения в кэш
bool LoadImageToCache(size_t index, CachedImage& cached) {
    if (index >= g_state.imageFiles.size()) {
        return false;
    }
    
    cached.loading = true;
    const std::string& filename = g_state.imageFiles[index];
    cached.filename = filename;
    
    int width, height;
    UniqueHBITMAP hBitmap = LoadPNGToBitmap(filename, width, height);
    
    if (!hBitmap) {
        cached.loading = false;
        return false;
    }
    
    HDC hdc = GetDC(nullptr);
    UniqueHDC hMemDC(CreateCompatibleDC(hdc));
    if (hMemDC) {
        SelectObject(hMemDC.get(), hBitmap.get());
    }
    ReleaseDC(nullptr, hdc);
    
    if (!hMemDC) {
        cached.loading = false;
        return false;
    }
    
    cached.hBitmap = std::move(hBitmap);
    cached.hMemDC = std::move(hMemDC);
    cached.width = width;
    cached.height = height;
    cached.loaded = true;
    cached.loading = false;
    
    return true;
}

// Загрузка изображения по индексу (синхронно)
bool LoadImageAtIndex(size_t index) {
    if (index >= g_state.imageFiles.size()) {
        return false;
    }

    static size_t loadCounter = 0;
    loadCounter++;
    if (loadCounter % 5 == 0) {
        // Принудительно выгружаем всё, кроме ближайших 8
        EvictOldImages(index, 8);
    }

    std::lock_guard<std::mutex> lock(g_state.cacheMutex);
    auto it = g_state.cache.find(index);
    
    if (it != g_state.cache.end() && it->second.loaded) {
        g_state.currentImage = &it->second;
        g_state.currentFile = it->second.filename;
        g_state.currentIndex = index;
        return true;
    }
    
    CachedImage& cached = g_state.cache[index];
    bool result = LoadImageToCache(index, cached);
    
    if (result) {
        g_state.currentImage = &cached;
        g_state.currentFile = cached.filename;
        g_state.currentIndex = index;
    }
    
    return result;
}

// Функция предзагрузки в отдельном потоке
void PreloadThreadFunc() {
    g_state.preloadRunning = true;
    
    while (!g_state.stopPreload) {
        std::unique_lock<std::mutex> lock(g_state.preloadMutex);
        g_state.preloadCV.wait(lock, []() { 
            return !g_state.preloadQueue.empty() || g_state.stopPreload; 
        });
        
        if (g_state.stopPreload) break;
        
        size_t index = g_state.preloadQueue.front();
        g_state.preloadQueue.pop();
        lock.unlock();
        
        {
            std::lock_guard<std::mutex> cacheLock(g_state.cacheMutex);
            auto it = g_state.cache.find(index);
            if (it != g_state.cache.end() && (it->second.loaded || it->second.loading)) {
                continue;
            }
        }
        
        {
            std::lock_guard<std::mutex> cacheLock(g_state.cacheMutex);
            CachedImage& cached = g_state.cache[index];
            LoadImageToCache(index, cached);
        }
    }
    
    g_state.preloadRunning = false;
}

// Запланировать предзагрузку следующих N изображений
void SchedulePreload(size_t fromIndex, size_t count) {
    {
        std::lock_guard<std::mutex> lock(g_state.preloadMutex);
        while (!g_state.preloadQueue.empty()) {
            g_state.preloadQueue.pop();
        }
    }
    
    size_t endIndex = std::min(fromIndex + count, g_state.imageFiles.size());
    for (size_t i = fromIndex; i < endIndex; ++i) {
        if (i == g_state.currentIndex) continue;
        
        bool alreadyLoaded = false;
        {
            std::lock_guard<std::mutex> lock(g_state.cacheMutex);
            auto it = g_state.cache.find(i);
            if (it != g_state.cache.end() && (it->second.loaded || it->second.loading)) {
                alreadyLoaded = true;
            }
        }
        
        if (!alreadyLoaded) {
            std::lock_guard<std::mutex> lock(g_state.preloadMutex);
            g_state.preloadQueue.push(i);
        }
    }
    
    g_state.preloadCV.notify_one();
}

// Сохранить текущий файл в best.txt
void SaveCurrentToBest() {
    if (g_state.currentFile.empty()) return;
    
    try {
        std::ofstream file("best.txt", std::ios::app);
        if (file.is_open()) {
            fs::path path(g_state.currentFile);
            file << path.filename().string() << "\n";
            file.close();
            
            std::string title = "PNG Viewer - Сохранено: " + path.filename().string();
            SetWindowTextA(g_state.hWnd, title.c_str());
            
            std::thread([hWnd = g_state.hWnd]() {
                Sleep(1500);
                char title[256];
                sprintf_s(title, "PNG Viewer [%zu/%zu]", 
                         g_state.currentIndex + 1, g_state.imageFiles.size());
                SetWindowTextA(hWnd, title);
            }).detach();
        }
    } catch (...) {}
}

// Удалить текущий файл
void DeleteCurrentFile() {
    if (g_state.currentFile.empty()) return;
    
    fs::path path(g_state.currentFile);
    std::string filename = path.filename().string();
    
    std::string msg = "Удалить файл?\n" + filename;
    int result = MessageBoxA(g_state.hWnd, msg.c_str(), "Подтверждение", 
                            MB_YESNO | MB_ICONQUESTION);
    
    if (result == IDYES) {
        try {
            // Удаляем файл
            fs::remove(g_state.currentFile);
            
            // Удаляем из списка
            size_t indexToRemove = g_state.currentIndex;
            g_state.imageFiles.erase(g_state.imageFiles.begin() + indexToRemove);
            
            // Удаляем из кэша
            {
                std::lock_guard<std::mutex> lock(g_state.cacheMutex);
                auto it = g_state.cache.find(indexToRemove);
                if (it != g_state.cache.end()) {
                    g_state.cache.erase(it);
                }
                
                // Обновляем индексы в кэше
                std::map<size_t, CachedImage> newCache;
                for (auto& pair : g_state.cache) {
                    if (pair.first > indexToRemove) {
                        newCache[pair.first - 1] = std::move(pair.second);
                    } else {
                        newCache[pair.first] = std::move(pair.second);
                    }
                }
                g_state.cache = std::move(newCache);
            }
            
            // Загружаем следующее или предыдущее
            size_t newIndex = indexToRemove;
            if (newIndex >= g_state.imageFiles.size()) {
                newIndex = g_state.imageFiles.size() - 1;
            }
            
            if (!g_state.imageFiles.empty()) {
                LoadImageAtIndex(newIndex);
                SchedulePreload(newIndex + 1, 8);
                InvalidateRect(g_state.hWnd, nullptr, FALSE);
                
                // Обновляем заголовок
                char title[256];
                sprintf_s(title, "PNG Viewer [%zu/%zu]", 
                         g_state.currentIndex + 1, g_state.imageFiles.size());
                SetWindowTextA(g_state.hWnd, title);
            }
            
        } catch (...) {
            MessageBoxA(g_state.hWnd, "Ошибка при удалении файла!", "Ошибка", MB_OK | MB_ICONERROR);
        }
    }
}

std::vector<std::string> FindPNGFiles(const std::string& folder,bool by_time=true) {
    std::vector<std::tuple<fs::file_time_type,uintmax_t, std::string>> files;
    
    try {
        for (const auto& entry : fs::directory_iterator(folder)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (ext == ".png") {
                    auto time = entry.last_write_time();
                    files.emplace_back(time,entry.file_size(),entry.path().string());
                }
            }
        }
    } catch (const fs::filesystem_error&) {}
    
    // Сортировка по времени изменения (от новых к старым)
    if(by_time)std::sort(files.begin(), files.end(),
              [](const auto& a, const auto& b) {
                  return std::get<0>(a) < std::get<0>(b);
              });
    if(!by_time)std::sort(files.begin(), files.end(),
              [](const auto& a, const auto& b) {
                  return std::get<1>(a) < std::get<1>(b);
              });
    
    // Возвращаем только пути
    std::vector<std::string> result;
    result.reserve(files.size());
    for (const auto& f : files) {
        result.push_back(std::get<2>(f));
    }
    
    return result;
}

// Найти индекс файла
size_t FindFileIndex(const std::vector<std::string>& files, const std::string& target) {
    if (target.empty()) return 0;
    
    // Ищем по полному пути
    for (size_t i = 0; i < files.size(); ++i) {
        if (files[i] == target) {
            return i;
        }
    }
    
    // Ищем по имени файла
    std::string targetName = GetFileNameFromPath(target);
    for (size_t i = 0; i < files.size(); ++i) {
        if (GetFileNameFromPath(files[i]) == targetName) {
            return i;
        }
    }
    
    return 0;
}

// Отрисовка с прогрессом загрузки (в back buffer)
void DrawImageToDC(HDC hdc, const RECT& rect) {
    if (!g_state.currentImage || !g_state.currentImage->loaded) {
        UniqueHBRUSH black(CreateSolidBrush(RGB(0, 0, 0)));
        FillRect(hdc, &rect, black.get());
        
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        
        UniqueHFONT font(CreateFontA(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                     CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                     DEFAULT_PITCH | FF_DONTCARE, "Arial"));
        HFONT oldFont = (HFONT)SelectObject(hdc, font.get());
        
        std::string status = "Загрузка... " + std::to_string(g_state.currentIndex + 1) + " / " + 
                            std::to_string(g_state.imageFiles.size());
        
        RECT textRect = rect;
        DrawTextA(hdc, status.c_str(), -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        
        SelectObject(hdc, oldFont);
        return;
    }
    
    CachedImage* img = g_state.currentImage;
    
    // Очищаем фон
    UniqueHBRUSH black(CreateSolidBrush(RGB(0, 0, 0)));
    FillRect(hdc, &rect, black.get());
    
    int winW = rect.right - rect.left;
    int winH = rect.bottom - rect.top;
    
    int drawW, drawH;
    int offsetX = 0, offsetY = 0;
    
    if (g_state.zoomMode == ZOOM_FIT) {
        // Fit - вписываем в окно
        float imgAspect = static_cast<float>(img->width) / img->height;
        float winAspect = static_cast<float>(winW) / winH;
        
        if (imgAspect > winAspect) {
            drawW = winW;
            drawH = static_cast<int>(winW / imgAspect);
            offsetY = (winH - drawH) / 2;
        } else {
            drawH = winH;
            drawW = static_cast<int>(winH * imgAspect);
            offsetX = (winW - drawW) / 2;
        }
    } else {
        // 100% - пиксель в пиксель
        drawW = img->width;
        drawH = img->height;
        
        // Центрируем
        if (drawW > winW || drawH > winH) {
            // Если картинка больше экрана, масштабируем до размера экрана
            float scaleX = static_cast<float>(winW) / drawW;
            float scaleY = static_cast<float>(winH) / drawH;
            float scale = std::min(scaleX, scaleY);
            drawW = static_cast<int>(drawW * scale);
            drawH = static_cast<int>(drawH * scale);
        }
        
        offsetX = (winW - drawW) / 2;
        offsetY = (winH - drawH) / 2;
    }
    
    SetStretchBltMode(hdc, HALFTONE);
    StretchBlt(hdc, rect.left + offsetX, rect.top + offsetY, drawW, drawH,
               img->hMemDC.get(), 0, 0, img->width, img->height, SRCCOPY);
    
    // Показываем информацию только если зажата F
    if (g_state.showInfo) {
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        
        UniqueHFONT font(CreateFontA(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                     CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                     DEFAULT_PITCH | FF_DONTCARE, "Consolas"));
        HFONT oldFont = (HFONT)SelectObject(hdc, font.get());
        
        // Информация в правом верхнем углу
        char info[512];
        fs::path path(g_state.currentFile);
        std::string filename = path.filename().string();
        
        sprintf_s(info, "[%zu/%zu]  %s  (%dx%d)  %s", 
                 g_state.currentIndex + 1, 
                 g_state.imageFiles.size(),
                 filename.c_str(),
                 img->width, img->height,
                 g_state.zoomMode == ZOOM_FIT ? "Fit" : "100%");
        
        // Рисуем с тенью для лучшей читаемости
        RECT infoRect = {rect.right - 450, rect.top + 15, rect.right - 15, rect.top + 45};
        
        // Полупрозрачный фон для текста
        RECT bgRect = infoRect;
        bgRect.bottom += 5;
        UniqueHBRUSH bgBrush(CreateSolidBrush(RGB(0, 0, 0)));
        SetBkMode(hdc, OPAQUE);
        SetBkColor(hdc, RGB(0, 0, 0));
        
        // Рисуем текст
        DrawTextA(hdc, info, -1, &infoRect, DT_RIGHT | DT_TOP | DT_SINGLELINE);
        
        // Дополнительная информация в левом верхнем углу
        char status[128];
        sprintf_s(status, "Zoom: %s  |  F: info  |  Del: delete  |  Space: zoom", 
                 g_state.zoomMode == ZOOM_FIT ? "Fit" : "100%");
        
        RECT statusRect = {rect.left + 15, rect.top + 15, rect.left + 500, rect.top + 45};
        SetTextColor(hdc, RGB(180, 180, 180));
        SelectObject(hdc, oldFont);
        
        UniqueHFONT smallFont(CreateFontA(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                         DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                         CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                         DEFAULT_PITCH | FF_DONTCARE, "Consolas"));
        SelectObject(hdc, smallFont.get());
        SetBkMode(hdc, TRANSPARENT);
        DrawTextA(hdc, status, -1, &statusRect, DT_LEFT | DT_TOP | DT_SINGLELINE);
        
        SelectObject(hdc, oldFont);
    }
}

// Оконная процедура
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            
            RECT rect;
            GetClientRect(hWnd, &rect);
            
            int winWidth = rect.right - rect.left;
            int winHeight = rect.bottom - rect.top;
            
            if (EnsureBackBuffer(winWidth, winHeight)) {
                DrawImageToDC(g_state.backBufferDC.get(), rect);
                BitBlt(hdc, 0, 0, winWidth, winHeight, 
                       g_state.backBufferDC.get(), 0, 0, SRCCOPY);
            } else {
                DrawImageToDC(hdc, rect);
            }
            
            EndPaint(hWnd, &ps);
            break;
        }
        
        case WM_KEYDOWN: {
            bool ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            
            switch (wParam) {
                case 'F': {
                    g_state.showInfo = true;
                    InvalidateRect(hWnd, nullptr, FALSE);
                    break;
                }
                
                case VK_SPACE: {
                    // Переключение режима масштабирования
                    g_state.zoomMode = (g_state.zoomMode == ZOOM_FIT) ? ZOOM_100 : ZOOM_FIT;
                    InvalidateRect(hWnd, nullptr, FALSE);
                    
                    // Показываем уведомление
                    std::string msg = g_state.zoomMode == ZOOM_FIT ? "Fit" : "100%";
                    std::string title = "PNG Viewer - " + msg;
                    SetWindowTextA(hWnd, title.c_str());
                    std::thread([hWnd]() {
                        Sleep(800);
                        char title[256];
                        sprintf_s(title, "PNG Viewer [%zu/%zu]", 
                                 g_state.currentIndex + 1, g_state.imageFiles.size());
                        SetWindowTextA(hWnd, title);
                    }).detach();
                    break;
                }
                
                case 'R': {
                    // Перезагрузить текущее изображение
                    if (!g_state.currentFile.empty()) {
                        size_t idx = g_state.currentIndex;
                        {
                            std::lock_guard<std::mutex> lock(g_state.cacheMutex);
                            auto it = g_state.cache.find(idx);
                            if (it != g_state.cache.end()) {
                                FreeCachedImage(it->second);
                            }
                        }
                        if (LoadImageAtIndex(idx)) {
                            SchedulePreload(idx + 1, 8);
                            InvalidateRect(hWnd, nullptr, FALSE);
                        }
                    }
                    break;
                }
                
                case VK_DELETE: {
                    DeleteCurrentFile();
                    break;
                }
                
                case VK_UP: {
                    SaveCurrentToBest();
                    break;
                }
                
                case VK_LEFT: {
                    if (g_state.currentIndex > 0) {
                        size_t step = ctrlPressed ? 10 : 1;
                        size_t newIndex = (g_state.currentIndex >= step) ? 
                                         g_state.currentIndex - step : 0;
                        if (LoadImageAtIndex(newIndex)) {
                            SchedulePreload(newIndex + 1, 8);
                            InvalidateRect(hWnd, nullptr, FALSE);
                        }
                    }
                    break;
                }
                
                case VK_RIGHT: {
                    if (g_state.currentIndex + 1 < g_state.imageFiles.size()) {
                        size_t step = ctrlPressed ? 10 : 1;
                        size_t newIndex = std::min(g_state.currentIndex + step, 
                                                  g_state.imageFiles.size() - 1);
                        if (LoadImageAtIndex(newIndex)) {
                            SchedulePreload(newIndex + 1, 8);
                            InvalidateRect(hWnd, nullptr, FALSE);
                        }
                    }
                    break;
                }
                
                case VK_PRIOR: {  // Page Up
                    if (g_state.currentIndex > 0) {
                        size_t newIndex = (g_state.currentIndex >= 10) ? 
                                         g_state.currentIndex - 10 : 0;
                        if (LoadImageAtIndex(newIndex)) {
                            SchedulePreload(newIndex + 1, 8);
                            InvalidateRect(hWnd, nullptr, FALSE);
                        }
                    }
                    break;
                }
                
                case VK_NEXT: {   // Page Down
                    if (g_state.currentIndex + 1 < g_state.imageFiles.size()) {
                        size_t newIndex = std::min(g_state.currentIndex + 10, 
                                                  g_state.imageFiles.size() - 1);
                        if (LoadImageAtIndex(newIndex)) {
                            SchedulePreload(newIndex + 1, 8);
                            InvalidateRect(hWnd, nullptr, FALSE);
                        }
                    }
                    break;
                }
                
                case VK_HOME: {
                    if (LoadImageAtIndex(0)) {
                        SchedulePreload(1, 8);
                        InvalidateRect(hWnd, nullptr, FALSE);
                    }
                    break;
                }
                
                case VK_END: {
                    if (!g_state.imageFiles.empty()) {
                        size_t last = g_state.imageFiles.size() - 1;
                        if (LoadImageAtIndex(last)) {
                            SchedulePreload(last + 1, 8);
                            InvalidateRect(hWnd, nullptr, FALSE);
                        }
                    }
                    break;
                }
                
                case VK_ESCAPE: {
                    SaveState();
                    PostQuitMessage(0);
                    break;
                }
            }
            break;
        }
        
        case WM_KEYUP: {
            if (wParam == 'F') {
                g_state.showInfo = false;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            break;
        }
        
        case WM_SETCURSOR: {
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
            return TRUE;
        }
        
        case WM_SIZE: {
            g_state.backBufferBitmap.reset();
            g_state.backBufferDC.reset();
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        }
        
        case WM_DESTROY: {
            SaveState();
            g_state.stopPreload = true;
            g_state.preloadCV.notify_all();
            if (g_state.preloadThread.joinable()) {
                g_state.preloadThread.join();
            }
            PostQuitMessage(0);
            break;
        }
            
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
#include <windows.h>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

// ----------------- Парсер аргументов -----------------

struct AppArgs {
    std::string folder;      // путь к папке (если не указан, берётся текущая)
    bool by_time = true;     // сортировать по времени (по умолчанию true)
    bool show_help = false;  // флаг --help
};

std::vector<std::string> split_args(const std::string& line) {
    std::vector<std::string> args;
    std::string cur;
    bool in_quote = false;
    for (char c : line) {
        if (c == '"' || c == '\'') {
            in_quote = !in_quote;
        } else if (c == ' ' && !in_quote) {
            if (!cur.empty()) {
                args.push_back(cur);
                cur.clear();
            }
        } else {
            cur += c;
        }
    }
    if (!cur.empty()) args.push_back(cur);
    return args;
}

AppArgs parse_args(const std::string& lpCmdLine) {
    AppArgs args;

    if (lpCmdLine.empty()) {
        // Если аргументов нет — текущая папка, by_time = true
        return args;
    }

    std::vector<std::string> tokens = split_args(lpCmdLine);

    for (size_t i = 0; i < tokens.size(); ++i) {
        std::string t = tokens[i];

        // --help
        if (t == "--help" || t == "-h" || t == "/h") {
            args.show_help = true;
            return args;
        }

        // --by_time, --by-time
        if (t == "--by_time" || t == "--by-time") {
            args.by_time = true;
            continue;
        }

        // --no_by_time, --no-by-time
        if (t == "--no_by_time" || t == "--no-by-time" || "--n"==t || "-n"==t || "0"==t) {
            args.by_time = false;
            continue;
        }

        // Если это просто путь (не флаг)
        if (t[0] != '-' && t[0] != '/') {
            args.folder = t;
            continue;
        }

        // Если не распознали — можно игнорировать или выводить ошибку
    }

    return args;
}

void print_help() {
    std::cout << "PNG Viewer - Version 1.0.0\n";
    std::cout << "\nUsage:\n";
    std::cout << "  png_viewer.exe [options] [folder_path]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --help, -h, /h          Show this help message and exit.\n";
    std::cout << "  --by_time, --by-time    Sort PNG files by modification time (default).\n";
    std::cout << "  --no_by_time, --no-by-time, --n, -n, 0\n";
    std::cout << "                          Sort PNG files by file size instead of time.\n";
    std::cout << "  folder_path             Path to folder with PNG files.\n";
    std::cout << "                          If not specified, current directory is used.\n\n";
    std::cout << "Examples:\n";
    std::cout << "  png_viewer.exe\n";
    std::cout << "  png_viewer.exe C:\\Pictures\n";
    std::cout << "  png_viewer.exe --no_by_time D:\\Images\n";
    std::cout << "  png_viewer.exe --help\n";
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Парсинг аргументов
    AppArgs args = parse_args(lpCmdLine ? lpCmdLine : "");

    // Если --help — вывести подсказку и выйти
    if (args.show_help) {
        print_help();
        return 0;
    }

    HCURSOR hWaitCursor = LoadCursor(nullptr, IDC_WAIT);
    SetCursor(hWaitCursor);

    char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);

    std::string folder = currentDir;
    if (!args.folder.empty()) {
        folder = args.folder;
    }

    bool by_time = args.by_time;  // по умолчанию true

    g_state.imageFiles = FindPNGFiles(folder, by_time);
    g_state.totalFiles = g_state.imageFiles.size();

    if (g_state.imageFiles.empty()) {
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
        MessageBoxA(nullptr, "PNG files not found in the folder!", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // Загружаем состояние
    std::string lastFile = LoadState();
    size_t startIndex = FindFileIndex(g_state.imageFiles, lastFile);
    
    // Если не нашли - ищем out_img172119.png
    if (startIndex == 0 && lastFile.empty()) {
        startIndex = FindFileIndex(g_state.imageFiles, "out_img172119.png");
    }
    
    // Регистрация класса окна
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = "PNGViewerClass";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    
    if (!RegisterClassExA(&wc)) {
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
        MessageBoxA(nullptr, "Ошибка регистрации класса окна!", "Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    HWND hWnd = CreateWindowExA(
        0,
        "PNGViewerClass",
        "PNG Viewer",
        WS_POPUP | WS_VISIBLE,
        0, 0, screenWidth, screenHeight,
        nullptr, nullptr, hInstance, nullptr
    );
    
    if (!hWnd) {
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
        MessageBoxA(nullptr, "Ошибка создания окна!", "Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    g_state.hWnd = hWnd;
    ShowWindow(hWnd, SW_MAXIMIZE);
    UpdateWindow(hWnd);
    
    g_state.preloadThread = std::thread(PreloadThreadFunc);
    
    if (!LoadImageAtIndex(startIndex)) {
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
        MessageBoxA(nullptr, "Не удалось загрузить изображение!", "Ошибка", MB_OK | MB_ICONERROR);
        g_state.stopPreload = true;
        g_state.preloadCV.notify_all();
        if (g_state.preloadThread.joinable()) {
            g_state.preloadThread.join();
        }
        return 1;
    }
    
    // Обновляем заголовок
    char title[256];
    sprintf_s(title, "PNG Viewer [%zu/%zu]", g_state.currentIndex + 1, g_state.imageFiles.size());
    SetWindowTextA(hWnd, title);
    
    SchedulePreload(startIndex + 1, 8);
    SetCursor(LoadCursor(nullptr, IDC_ARROW));
    
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    g_state.stopPreload = true;
    g_state.preloadCV.notify_all();
    if (g_state.preloadThread.joinable()) {
        g_state.preloadThread.join();
    }
    
    g_state.cache.clear();
    
    return 0;
}