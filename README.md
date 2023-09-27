WIP
## Installation
### 1. Installing CGAL with Vcpkg
https://doc.cgal.org/latest/Manual/windows.html
```
> ./vcpkg.exe install cgal:x64-windows
```

### 2. Installing Ceres with Vcpkg
http://ceres-solver.org/installation.html
```
> ./vcpkg.exe install ceres:x64-windows
> ./vcpkg.exe integrate install
```
  
### 3. Build
#### CMake
```
> mkdir build
> cd build
> cmake -DCMAKE_TOOLCHAIN_FILE=VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake -DCMAKE_PREFIX_PATH="HOUDINI_ROOT\toolkit\cmake" ..
> cmake --build . --clean-first
```
#### CMake Presets
Rewrite `HOUDINI_ROOT` in `CMakePresets.json`  
```
...
"environment": {
    "HOUDINI_ROOT": "C:/Program Files (x86)/Steam/steamapps/common/Houdini Indie"
},
...
```  
```
> cmake --preset release-visual-studio-windows-x64
> cmake --build --preset build-release-visual-studio-windows-x64
```
  
#### VSCode CMake tools
.vscode/settings.json
```
...
"cmake.useCMakePresets": "always",
...
```
`CMake: Select Configure Preset`  
`CMake: Configure`  
`CMake: Select Build Preset`  
`CMake: Build`  

### 4. houdini.env
```
...
PATH = $PATH;$HOUDINI_USER_PREF_DIR/bin
```