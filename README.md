# Design Studio (C++ / WebAssembly)

- `core.cpp`  – all design logic in C++ (HTML/CSS generation, escaping, colour sanitising, GitHub URLs)
- `index.html` – browser interface; calls the compiled core
- `build.sh`  – compiles `core.cpp` to `design_core.js` (WebAssembly embedded)
- `.github/workflows/pages.yml` – builds and deploys the app to GitHub Pages on every push to `main`

## Build and run locally
1. Install Emscripten: https://emscripten.org/docs/getting_started/downloads.html (then `source ./emsdk_env.sh`)
2. `bash build.sh`
3. `python3 -m http.server 8000` and open http://localhost:8000

## Collaborate on GitHub
1. Create a repo, push these files (`design_core.js` is generated, so add it to `.gitignore`).
2. Settings > Collaborators (or the app's "Invite friends" button) > Add people.
3. Everyone works on branches and opens Pull Requests; merge into `main`.
4. Settings > Pages > Source: GitHub Actions. Every merge redeploys the app.
