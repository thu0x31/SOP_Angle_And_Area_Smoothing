## Installation
#### 1. Installing CGAL with Vcpkg
https://doc.cgal.org/latest/Manual/windows.html
```
.\vcpkg.exe install cgal:x64-windows
```
  
#### 2. Build
##### CMake
```
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake  -DCMAKE_PREFIX_PATH="HOUDINI_ROOT\toolkit\cmake" ..
cmake --build . --clean-first
```
  
##### VSCode CMake tools
`CMake: Select Configure Preset`
`CMake: Configure`
`CMake: Select Build Preset`
`CMake: Build`

#### 3. houdini.env
```
...
PATH = $PATH;$HOUDINI_USER_PREF_DIR/bin
```