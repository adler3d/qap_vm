@pushd ..
@call emsdk_env.bat
@popd
emcc main.cpp -std=c++11 -O2 -s TOTAL_MEMORY=258435456 -o hello.html -s ASYNCIFY=1 --shell-file minshell.html
