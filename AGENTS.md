# AGENTS.md — Guidance for coding agents working on Funkin++

This file documents the essential, discoverable information an AI coding agent needs to be productive in this repository. Keep answers precise and reference concrete files/examples.

What this repo is (quick):
- C++ rewrite of Friday Night Funkin' with a small custom Engine (DLL/static library) and a Game executable. There is no separate `Launcher` project in this workspace.
- Top-level projects: `Source/Engine` (core library), `Source/Game` (game binary).

Key files and where to look (quick map):
- Build & workspace: `premake5.lua` (root) — primary project meta / configurations; `Funkin++.slnx` (generated/openable Visual Studio solution).
- Engine core: `Source/Engine/` — `FunkinPCH.h` (precompiled header, required includes), `Core/CoreDefines.h` (export macros: `FUNKIN_API`, platform macros).
-- Platform abstraction: this repository does not currently include a separate `Source/Engine/Platform` layer. Platform-specific code (e.g., Win32 entry) is used directly in `Source/Game/FunkinMain.cpp` and the Engine sources. If a platform layer is added later it would live under `Source/Engine/Platform`.
-- Game entry point: `Source/Game/FunkinMain.cpp` — this is the canonical program entry that calls into the Engine (`FunkinEngineMain`).
- Build output and runtime artifacts: `Binaries/Win64/` and `Intermediate/Win64/`.

Big-picture architecture (short):
-- Single workspace with multiple targets: Engine (core library) → consumed by Game.
- Platform layer: engine exposes a thin platform abstraction (IGenericPlatformProcess) and platform-specific `CPlatformProcess` typedef is chosen in `PlatformProcess.h` via `FUNKIN_PLATFORM_*` defines.
- Launcher is responsible for ensuring single-instance behavior, resolving paths relative to its exe, creating a named mutex and spawning the game executable (see `FunkinLauncherMain.cpp`). The Game binary expects to be launched via the Launcher — it checks for the same named mutex and will show an error dialog and exit if launched directly.

Project-specific conventions and patterns (actionable, concrete):
- Naming conventions: Types prefixed with C (classes), F (plain structs/handles), I (interface/abstract base); functions and methods use PascalCase (e.g., `CreateNamedMutex`, `SpawnProcess`).
-- Platform abstraction pattern: currently there is no platform shim in the repo; platform-specific behavior is implemented inline where needed. Agents should inspect `Source/Game/FunkinMain.cpp` and Engine source files for direct platform calls (e.g., Win32 includes).
-- Mutex/launch pattern: there is no Launcher-enforced named mutex in the current codebase — the Game entrypoint (`FunkinMain.cpp`) calls into the Engine directly.
-- Precompiled headers: `Source/Engine/FunkinPCH.h` is used; new Engine source files should include this PCH to match project settings and avoid build errors.
-- Export macros: `FUNKIN_API` in `Source/Engine/Core/CoreDefines.h` controls DLL import/export — when modifying Engine public headers be mindful of `FUNKIN_EXPORT_LIBRARY` usage in build.

Build / run / debug workflows (Windows, discovered):
- Generate Visual Studio projects (recommended for development/debugging):

  powershell
  .\premake5.exe vs2022
  start .\Funkin++.slnx

- Build from command line (MSBuild) once solution is generated:

  powershell
  msbuild .\Funkin++.slnx /p:Configuration=Debug /m

-- Quick/run existing built binaries (no build):
  - Game: `Binaries\Win64\FunkinPlusPlus-Win64-Debug.exe` (or the same filename for other configurations) — run this directly to start the game. There is no separate Launcher binary in the current workspace.

Debugging tips specific to this repo:
- To debug the Game under Visual Studio: set `Game` as the startup project (premake already sets `startproject "Game"`) and run under the debugger, or open the generated solution and attach to the running `FunkinPlusPlus-Win64-Debug.exe` process.
- Because there is no Launcher-enforced mutex in this workspace, you can run the game executable directly when debugging.

Integration points & external dependencies (discoverable):
- Build system: premake5 is provided at repo root. Generated solution targets MSVC on Windows. The premake script sets `cppdialect = "C++20"` and defines platform-specific macros like `FUNKIN_PLATFORM_WIN64`.
- No third-party package manager files (no vcpkg/Conan manifests present in repo root). Third-party dependencies (if any) should be under `Source/ThirdParty` per `premake5.lua` expectations.
- Engine exports: `FUNKIN_API` controls dllimport/dllexport — Engine is intended to be built as a library and linked by the Game/Launcher.

Quick examples to reference while editing code:
- Platform selection typedef: `Source/Engine/Platform/PlatformProcess.h` (lines where `typedef CWindowsPlatformProcess CPlatformProcess;` is defined).
-- Example entry point: `Source/Game/FunkinMain.cpp` (calls `FunkinEngineMain`).
-- PCH usage: `Source/Engine/FunkinPCH.h` — include order and Windows header inclusion under `FUNKIN_PLATFORM_WIN64`.

What agents should NOT assume:
- There is no test harness in the repo — do not search for unit-test targets.
-- The game may be launched directly in this workspace (no Launcher mutex enforced). Tests and agents should not assume a separate Launcher is required.

If you modify build settings or add files:
- Update `premake5.lua` if you need new include directories, files, or new projects; run `.\premake5.exe vs2022` to regenerate the solution.
- Add new Engine public headers guarded with `FUNKIN_API` as appropriate; ensure new source files include `FunkinPCH.h` unless intentionally skipping PCH.

Where to look next (for deeper agent tasks):
- Any added platform functionality: `Source/Engine/Platform/*`
- Engine public API: headers added under `Source/Engine/` and `Core/`
 - Engine/game data or asset loading — search for "Assets", "Content", or filesystem usage in `Source/Engine` and `Source/Game`.

End of AGENTS.md

