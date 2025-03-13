# Installation Guide

## Dependencies
To compile and run MacroDeck, the following dependencies are required:

- **CMake** - Configures the build system
- **Ninja** – Compiles the source code
- **libX11** - X11 window system interaction
- **libXtst** - Keyboard emulation and keybinds
- **libXKBcommon** - Keyboard extension
- **ALSA** – Advanced Linux Sound Architecture for sound manipulation
- **nlohmann-json** – Handles config and macro files
- **Crow** – Web server framework (requires Boost for async operations)
- **Boost** – Required by Crow for async functionality

## Building from Source
### Installing Required Libraries

**Arch Linux**
```sh
pacman -S libx11 libxtst libxkbcommon alsa-lib boost
```

**Ubuntu/Debian**
```sh
apt install libx11-dev libxtst-dev libxkbcommon0 libasound2-dev libboost-all-dev
```

### Compiling MacroDeck
```sh
git clone https://github.com/vh8t/MacroDeck.git
cd MacroDeck

mkdir build
cd build

cmake -G Ninja ..
ninja

cd ..
./build/MacroDeck
```

This will compile and launch MacroDeck. Ensure all dependencies are installed before running the build process.
