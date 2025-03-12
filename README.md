# MacroDeck

MacroDeck is an open-source alternative to Stream Deck. Instead of using a physical button box, MacroDeck runs a web server on your local network, allowing you to connect via your phone and use it as a macro deck.

## Features
- Open-source and customizable
- Runs a local web server for easy remote access
- Supports custom macros using YAML configuration files
- Works on Linux with X11 (Wayland and Windows support planned)
- Keyboard emulation and keybindings using X11
- Sound manipulation with ALSA

## Installation
### Dependencies
To compile and run MacroDeck, the following dependencies are required:

- **CMake** (for configuring the build system)
- **Ninja** (for compiling the source code)
- **libX11** (for X11 window system interaction)
- **libXtst** (for keyboard emulation and keybinds)
- **libXKBcommon** (for keyboard extension)
- **ALSA** (Advanced Linux Sound Architecture for sound manipulation)
- **nlohmann-json** (for handling config and macro files)
- **Crow** (for the web server, requires Boost for async operations)
- **Boost** (required by Crow for async functionality)

### Building from source
#### Installing libraries

Arch:
```sh
pacman -S libx11 libxtst libxkbcommon alsa-lib boost
```

Ubuntu:
```sh 
apt install libx11-dev libxtst-dev libxkbcommon0 libacound2-dev libboost-all-dev
```

#### Compiling source code
```sh
git clone https://github.com/vh8t/MacroDeck.git
cd MacroDeck

mkdir build
cd build

cmake -G Ninja ..
ninja

./MacroDeck
```

## Usage
Once MacroDeck is running, it will start a web server on your local network. The IP address and port will be displayed in the terminal. Connect using your phone’s browser and start using your custom macros.

## Future Plans
- Expand support to **Wayland**
- Add compatibility with **Windows**

## Contributing
Contributions are welcome! Feel free to fork the repository, submit issues, and create pull requests.

## License
MacroDeck is licensed under the Apache 2.0 License. See [LICENSE](LICENSE) for details.
