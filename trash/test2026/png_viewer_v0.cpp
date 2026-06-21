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
    
    // Запрещаем копирование
    CachedImage(const CachedImage&) = delete;
    CachedImage& operator=(const CachedImage&) = delete;
    
    // Разрешаем перемещение
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
    
    // Для статистики
    size_t totalFiles = 0;
};

ViewerState g_state;

// Освобождение кэшированного изображения (больше не нужен, т.к. умные указатели)
// Но оставляем для совместимости, теперь просто сбрасываем
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
    
    // Освобождаем старый буфер
    g_state.backBufferBitmap.reset();
    g_state.backBufferDC.reset();
    
    // Создаём новый
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
    bmi.bmiHeader.biHeight = -height; // Top-down
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
        std::swap(image[i], image[i + 2]);  // R<->B
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
    
    // Создаём HDC для bitmap
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
    
    // Перемещаем ресурсы в кэш
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
    
    // Проверяем кэш
    std::lock_guard<std::mutex> lock(g_state.cacheMutex);
    auto it = g_state.cache.find(index);
    
    if (it != g_state.cache.end() && it->second.loaded) {
        g_state.currentImage = &it->second;
        g_state.currentFile = it->second.filename;
        g_state.currentIndex = index;
        return true;
    }
    
    // Если нет в кэше - загружаем синхронно
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
        
        // Проверяем, нужно ли загружать
        {
            std::lock_guard<std::mutex> cacheLock(g_state.cacheMutex);
            auto it = g_state.cache.find(index);
            if (it != g_state.cache.end() && (it->second.loaded || it->second.loading)) {
                continue;
            }
        }
        
        // Загружаем
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
    // Очищаем очередь
    {
        std::lock_guard<std::mutex> lock(g_state.preloadMutex);
        while (!g_state.preloadQueue.empty()) {
            g_state.preloadQueue.pop();
        }
    }
    
    // Добавляем следующие изображения
    size_t endIndex = std::min(fromIndex + count, g_state.imageFiles.size());
    for (size_t i = fromIndex; i < endIndex; ++i) {
        // Пропускаем текущее изображение
        if (i == g_state.currentIndex) continue;
        
        // Проверяем, не загружено ли уже
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
    if (g_state.currentFile.empty()) {
        return;
    }
    
    try {
        std::ofstream file("best.txt", std::ios::app);
        if (file.is_open()) {
            // Извлекаем только имя файла без пути
            fs::path path(g_state.currentFile);
            file << path.filename().string() << "\n";
            file.close();
            
            // Показываем уведомление в заголовке окна
            std::string title = "PNG Viewer - Сохранено: " + path.filename().string();
            SetWindowTextA(g_state.hWnd, title.c_str());
            
            // Возвращаем нормальный заголовок через 1.5 секунды
            std::thread([hWnd = g_state.hWnd]() {
                Sleep(1500);
                SetWindowTextA(hWnd, "PNG Viewer");
            }).detach();
        }
    } catch (...) {
        // Игнорируем ошибки записи
    }
}

// Найти все PNG файлы в папке
std::vector<std::string> FindPNGFiles(const std::string& folder) {
    std::vector<std::string> files;
    try {
        for (const auto& entry : fs::directory_iterator(folder)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (ext == ".png") {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const fs::filesystem_error&) {
        // Игнорируем
    }
    
    std::sort(files.begin(), files.end());
    return files;
}

// Найти индекс файла
size_t FindStartingIndex(const std::vector<std::string>& files, const std::string& target) {
    for (size_t i = 0; i < files.size(); ++i) {
        if (files[i].find(target) != std::string::npos) {
            return i;
        }
    }
    return 0;
}

// Отрисовка с прогрессом загрузки (в back buffer)
void DrawImageToDC(HDC hdc, const RECT& rect) {
    if (!g_state.currentImage || !g_state.currentImage->loaded) {
        // Очищаем фон (используем умный указатель)
        UniqueHBRUSH black(CreateSolidBrush(RGB(0, 0, 0)));
        FillRect(hdc, &rect, black.get());
        
        // Показываем статус загрузки
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
    
    // Масштабирование с сохранением пропорций
    int winW = rect.right - rect.left;
    int winH = rect.bottom - rect.top;
    
    float imgAspect = static_cast<float>(img->width) / img->height;
    float winAspect = static_cast<float>(winW) / winH;
    
    int drawW, drawH;
    int offsetX = 0, offsetY = 0;
    
    if (imgAspect > winAspect) {
        drawW = winW;
        drawH = static_cast<int>(winW / imgAspect);
        offsetY = (winH - drawH) / 2;
    } else {
        drawH = winH;
        drawW = static_cast<int>(winH * imgAspect);
        offsetX = (winW - drawW) / 2;
    }
    
    // Очищаем фон (чёрный)
    UniqueHBRUSH black(CreateSolidBrush(RGB(0, 0, 0)));
    FillRect(hdc, &rect, black.get());
    
    // Рисуем изображение
    SetStretchBltMode(hdc, HALFTONE);
    StretchBlt(hdc, rect.left + offsetX, rect.top + offsetY, drawW, drawH,
               img->hMemDC.get(), 0, 0, img->width, img->height, SRCCOPY);
    
    // Показываем номер файла в углу
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(200, 200, 200));
    
    UniqueHFONT font(CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                 CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                 DEFAULT_PITCH | FF_DONTCARE, "Arial"));
    HFONT oldFont = (HFONT)SelectObject(hdc, font.get());
    
    char info[256];
    sprintf_s(info, "%zu / %zu", g_state.currentIndex + 1, g_state.imageFiles.size());
    RECT infoRect = {rect.left + 15, rect.top + 15, rect.right - 15, rect.top + 45};
    DrawTextA(hdc, info, -1, &infoRect, DT_LEFT | DT_TOP);
    
    SelectObject(hdc, oldFont);
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
            
            // Создаём back buffer если нужно
            if (EnsureBackBuffer(winWidth, winHeight)) {
                // Рисуем в back buffer
                DrawImageToDC(g_state.backBufferDC.get(), rect);
                // Копируем на экран
                BitBlt(hdc, 0, 0, winWidth, winHeight, 
                       g_state.backBufferDC.get(), 0, 0, SRCCOPY);
            } else {
                // Fallback - рисуем напрямую
                DrawImageToDC(hdc, rect);
            }
            
            EndPaint(hWnd, &ps);
            break;
        }
        
        case WM_KEYDOWN: {
            switch (wParam) {
                case VK_UP: {
                    SaveCurrentToBest();
                    break;
                }
                
                case VK_LEFT:
                case VK_PRIOR: {  // Page Up
                    if (g_state.currentIndex > 0) {
                        size_t newIndex = g_state.currentIndex - 1;
                        if (LoadImageAtIndex(newIndex)) {
                            SchedulePreload(newIndex + 1, 8);
                            InvalidateRect(hWnd, nullptr, FALSE);
                            SetCursor(LoadCursor(nullptr, IDC_ARROW));
                        }
                    }
                    break;
                }
                    
                case VK_RIGHT:
                case VK_NEXT: {   // Page Down
                    if (g_state.currentIndex + 1 < g_state.imageFiles.size()) {
                        size_t newIndex = g_state.currentIndex + 1;
                        if (LoadImageAtIndex(newIndex)) {
                            SchedulePreload(newIndex + 1, 8);
                            InvalidateRect(hWnd, nullptr, FALSE);
                            SetCursor(LoadCursor(nullptr, IDC_ARROW));
                        }
                    }
                    break;
                }
                    
                case VK_HOME: {
                    if (LoadImageAtIndex(0)) {
                        SchedulePreload(1, 8);
                        InvalidateRect(hWnd, nullptr, FALSE);
                        SetCursor(LoadCursor(nullptr, IDC_ARROW));
                    }
                    break;
                }
                    
                case VK_END: {
                    if (!g_state.imageFiles.empty()) {
                        size_t last = g_state.imageFiles.size() - 1;
                        if (LoadImageAtIndex(last)) {
                            InvalidateRect(hWnd, nullptr, FALSE);
                            SetCursor(LoadCursor(nullptr, IDC_ARROW));
                        }
                    }
                    break;
                }
                    
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    break;
            }
            break;
        }
        
        case WM_SETCURSOR: {
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
            return TRUE;
        }
        
        case WM_SIZE: {
            // При изменении размера пересоздаём back buffer
            g_state.backBufferBitmap.reset();
            g_state.backBufferDC.reset();
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        }
        
        case WM_DESTROY: {
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Устанавливаем курсор ожидания только на время инициализации
    HCURSOR hWaitCursor = LoadCursor(nullptr, IDC_WAIT);
    SetCursor(hWaitCursor);
    
    // Получаем текущую папку
    char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);
    
    std::string folder = currentDir;
    if (lpCmdLine && lpCmdLine[0]) {
        folder = lpCmdLine;
    }
    
    // Находим все PNG файлы
    g_state.imageFiles = FindPNGFiles(folder);
    g_state.totalFiles = g_state.imageFiles.size();
    
    if (g_state.imageFiles.empty()) {
        SetCursor(LoadCursor(nullptr, IDC_ARROW));
        MessageBoxA(nullptr, "PNG файлы не найдены в папке!", "Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // Начинаем с out_img172119.png или с первого файла
    size_t startIndex = FindStartingIndex(g_state.imageFiles, "out_img172119.png");
    if (startIndex >= g_state.imageFiles.size()) {
        startIndex = 0;
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
    
    // Создание полноэкранного окна
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
    
    // Запускаем поток предзагрузки
    g_state.preloadThread = std::thread(PreloadThreadFunc);
    
    // Загружаем начальное изображение
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
    
    // Планируем предзагрузку следующих 8
    SchedulePreload(startIndex + 1, 8);
    
    // Возвращаем нормальный курсор
    SetCursor(LoadCursor(nullptr, IDC_ARROW));
    
    // Главный цикл сообщений
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Очистка
    g_state.stopPreload = true;
    g_state.preloadCV.notify_all();
    if (g_state.preloadThread.joinable()) {
        g_state.preloadThread.join();
    }
    
    // Очищаем кэш (умные указатели всё сделают сами)
    g_state.cache.clear();
    
    return 0;
}