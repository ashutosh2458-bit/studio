#!/usr/bin/env bash
# Compiles core.cpp to WebAssembly. Requires the Emscripten SDK (emcc).
# Output: design_core.js (the .wasm is embedded, so one file is enough).
set -euo pipefail
emcc core.cpp -O2 -std=c++17 -o design_core.js \
  -sMODULARIZE=1 -sEXPORT_NAME=createCore -sSINGLE_FILE=1 -sENVIRONMENT=web \
  -sALLOW_MEMORY_GROWTH=1 \
  -sEXPORTED_FUNCTIONS=_build_design,_get_full,_get_css,_get_linked,_github_new_repo_url,_github_file_url,_github_invite_url \
  -sEXPORTED_RUNTIME_METHODS=cwrap
echo "Built design_core.js - now run: python3 -m http.server 8000  and open http://localhost:8000"
