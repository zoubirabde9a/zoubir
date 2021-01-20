
em++ -std=c++14 "..\code\emscripten_app.cpp" -msimd128 -msse2 -s WASM=0 -s USE_WEBGL2=1 -s USE_SDL=2 -s FULL_ES3=1 -DCOMPILER_EMSCRIPTEN -DAPP_DEV -DEMCC_DEBUG=1 -Wno-write-strings --memory-init-file 0 -O0 -o build\plain.html
