# Computer Graphics HW1

A Computer Graphics homework project using OpenGL and GLFW for window management and rendering.

## Prerequisites

### Windows
- **CMake** (version 3.10 or higher)
- **Visual Studio 2019/2022** with C++ development tools
- **Git** (if cloning the repository)

### macOS
- **CMake** (version 3.10 or higher)
- **Xcode Command Line Tools**
- **Git** (if cloning the repository)

### Linux
- **CMake** (version 3.10 or higher)
- **GCC/Clang** compiler with C++11 support
- **OpenGL development libraries**
- **X11 development libraries**
- **Git** (if cloning the repository)

## Project Structure

```
HW1/
├── CMakeLists.txt          # Main CMake configuration
├── README.md               # This file
├── build/                  # Build directory (created during build)
├── deps/
│   └── glfw-3.4/          # GLFW library (included)
└── src/
    ├── config.h           # Project headers and includes
    └── main.cpp           # Main application source
```

## Building the Project

### Windows (PowerShell/Command Prompt)

1. **Navigate to the project directory:**
   ```powershell
   cd "path\to\HW1"
   ```

2. **Create and enter the build directory:**
   ```powershell
   mkdir build -ErrorAction SilentlyContinue
   cd build
   ```

3. **Configure the project with CMake:**
   ```powershell
   cmake ..
   ```

4. **Build the project:**
   ```powershell
   cmake --build . --config Release
   ```

5. **Run the executable:**
   ```powershell
   .\Release\hw1.exe
   ```

### macOS/Linux (Terminal)

1. **Navigate to the project directory:**
   ```bash
   cd /path/to/HW1
   ```

2. **Create and enter the build directory:**
   ```bash
   mkdir -p build
   cd build
   ```

3. **Configure the project with CMake:**
   ```bash
   cmake ..
   ```

4. **Build the project:**
   ```bash
   cmake --build . --config Release
   # or alternatively:
   make -j$(nproc)
   ```

5. **Run the executable:**
   ```bash
   ./hw1
   ```

## Quick Build Script

### Windows (PowerShell)
```powershell
# Save as build.ps1
Set-Location $PSScriptRoot
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path build
Set-Location build
cmake ..
cmake --build . --config Release
Write-Host "Build complete! Run with: .\Release\hw1.exe"
```

### macOS/Linux (Bash)
```bash
#!/bin/bash
# Save as build.sh
cd "$(dirname "$0")"
rm -rf build
mkdir build
cd build
cmake ..
cmake --build . --config Release
echo "Build complete! Run with: ./hw1"
```

## Rebuilding After Code Changes

If you modify source files (`src/main.cpp`, `src/config.h`), you only need to rebuild:

### Windows
```powershell
cd build
cmake --build . --config Release
```

### macOS/Linux
```bash
cd build
cmake --build . --config Release
# or: make
```

## Cleaning the Build

To perform a clean rebuild:

### Windows
```powershell
cd build
Remove-Item * -Recurse -Force -ErrorAction SilentlyContinue
cmake ..
cmake --build . --config Release
```

### macOS/Linux
```bash
cd build
rm -rf * 
cmake ..
cmake --build . --config Release
```

## Dependencies

This project includes all necessary dependencies:

- **GLFW 3.4**: Window management and input handling (included in `deps/glfw-3.4/`)
- **OpenGL**: Graphics rendering (system library)

The CMake configuration automatically:
- Builds GLFW from source
- Links OpenGL libraries appropriately for your platform
- Sets up include directories
- Configures compiler settings

## Troubleshooting

### Common Issues

1. **CMake not found**: Install CMake and ensure it's in your PATH
2. **Compiler not found**: 
   - Windows: Install Visual Studio with C++ tools
   - macOS: Install Xcode Command Line Tools (`xcode-select --install`)
   - Linux: Install build-essential (`sudo apt install build-essential`)

3. **OpenGL libraries not found** (Linux):
   ```bash
   # Ubuntu/Debian
   sudo apt install libgl1-mesa-dev libglu1-mesa-dev
   
   # Fedora/CentOS
   sudo yum install mesa-libGL-devel mesa-libGLU-devel
   ```

4. **X11 libraries not found** (Linux):
   ```bash
   # Ubuntu/Debian
   sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
   
   # Fedora/CentOS
   sudo yum install libX11-devel libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel
   ```

### Debug Build

For debugging, use Debug configuration:

```bash
cmake --build . --config Debug
```

The debug executable will be in:
- Windows: `build/Debug/hw1.exe`
- macOS/Linux: `build/hw1`

## Development

- **Language**: C++11
- **Graphics API**: OpenGL
- **Window Management**: GLFW 3.4
- **Build System**: CMake 3.10+

## License

This project is for educational purposes as part of a Computer Graphics course.