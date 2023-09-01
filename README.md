# Torrent Client

---

## Contents

1. [Project idea](#project-idea)
2. [How To Build](#how-to-build)
3. [Qt Version](#qt-version)
4. [Command-line](#command-line)
5. [Libraries Used](#libraries-used)
6. [TODO](#todo)

---

### Project idea
The main goal of writing a multithreaded Torrent Client in C++ is to gain in-depth knowledge of networking and peer-to-peer protocols while having the flexibility to create either a command-line or graphical user interface version using Qt.

---

### How To Build
To build library (libtorrent_core.a) and two programs (command-line and Qt), simply use the `make` command in your terminal (you may have to install curl and libcrypto).

- **torrent_client** - command-line program.
- **Torrent Client Qt** - graphical user interface (GUI).

Developed on Debian 12.

---

### Qt Version
![QT_version](https://github.com/punkertron/Torrent_Client_CPP/assets/82904352/a28586e4-32f6-4984-abf5-d2feb4e02f33)

---

### Command-line
![terminal_version](https://github.com/punkertron/Torrent_Client_CPP/assets/82904352/4c15d61f-56cb-43f4-9f55-37d254e40f2c)

The program supports the following commandline options:

| Options | Alternative | Description                             | Default            |
|---------|-------------|-----------------------------------------|--------------------|
| -t      | --torrent   | Location of the .torrent                | REQUIRED           |
| -d      | --directory | Where to save file                      | ~/Downloads (or current directory if ~/Downloads doesn't exists)           |
| -l      | --logs      | Enable logs (./logs/logs.txt)           | false              |
| -h      | --help      | Print arguments and their descriptions  |                    |

---

### Libraries Used
- [**Qt**](https://www.qt.io/): C++ framework for creating graphical user interfaces.
- [**libcurl**](https://curl.se/libcurl/): Use curl to retrieve peer list.
- [**Crypto++**](https://www.cryptopp.com/): For creating sha1.
- [**bencode.hpp**](https://github.com/jimporter/bencode.hpp): Lightweight lib for parsing and generating [**bencoded**](https://en.wikipedia.org/wiki/Bencode) data.
- [**spdlog**](https://github.com/gabime/spdlog): C++ logging library.
- [**cxxopts**](https://github.com/jarro2783/cxxopts): Lightweight C++ command line option parser.

---

### TODO
- Seeding.
- Resuming a download.
- Downloading multi-file Torrents.
- UDP support when receiving a list of peers.
