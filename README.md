# MacroDeck

MacroDeck is an open-source alternative to Stream Deck. Instead of using a physical button box, MacroDeck runs a web server on your local network, allowing you to connect via your phone and use it as a macro deck.

## Features
- Open-source and customizable
- Runs a local web server for easy remote access
- Supports custom macros using YAML configuration files
- Works on Linux with X11 and Wayland (Windows support planned)
- Keyboard emulation and keybindings
- Sound manipulation

## Installation
### Dependencies
To compile and run MacroDeck, the following dependencies are required:

- **CMake** - Configures the build system
- **Ninja** – Compiles the source code
- **ALSA** – Advanced Linux Sound Architecture for sound manipulation
- **nlohmann-json** – Handles config and macro files
- **Crow** – Web server framework (requires Boost for async operations)
- **Boost** – Required by Crow for async functionality

### Building from source
#### Installing libraries

**Arch Linux**
```sh
pacman -S alsa-lib boost
```

**Ubuntu/Debian**
```sh
apt install libasound2-dev libboost-all-dev
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

## Config
- All documentation can be found [here](https://github.com/vh8t/MacroDeck/wiki)

## Future Plans
- Add compatibility with **Windows**

## Contributing
Contributions are welcome! Feel free to fork the repository, submit issues, and create pull requests.

## License
MacroDeck is licensed under the Apache 2.0 License. See [LICENSE](LICENSE) for details.
