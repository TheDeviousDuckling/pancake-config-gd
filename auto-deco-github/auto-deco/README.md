# Auto Deco (Geode mod)

In the editor: select solid blocks -> pick a theme (Acheron / Space / Water / Grasslands) -> "Auto Deco".

## Build a .geode WITHOUT a GitHub account
1. Install the Geode CLI + SDK from https://geode-sdk.org (installer or `geode sdk install`).
2. Windows: install Visual Studio Build Tools (C++ workload). Mac: Xcode CLT.
3. In this folder:  `geode build`
4. Your file appears in `build/` as `yourname.auto-deco.geode`. Drop it in GD's `geode/mods` folder.

Edit `mod.json` ("id", "developer") and the placeholder object IDs in `src/main.cpp` first.

## Build with GitHub Actions
1. Create a new GitHub repo and push this folder to it (the `.github/workflows/build.yml` file must be included).
2. Open the repo's **Actions** tab. A build starts on every push (or run "Build Geode Mod" manually).
3. When it finishes, open the run and download the **Build Output** artifact. It contains one combined `.geode` for Windows, Mac, Android and iOS.
