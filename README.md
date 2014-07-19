#gTorrent
[![Build Status](https://travis-ci.org/gtorrent/gTorrent.svg?branch=master)](https://travis-ci.org/gtorrent/gTorrent)
[![Screenshot](http://puu.sh/afhPf/1224d5367d.png)](http://puu.sh/afhPf/1224d5367d.png)
[Screencast (old version)](http://a.pomf.se/lkowau.webm)

PULL REQUESTS ARE ENCOURAGED

## About
A minimalistic lightweight bittorrent client written by some smart people from the 4chan technology board.

### References
Links to the documentation of each library.
- [gtkmm](https://developer.gnome.org/gtkmm/3.12/)
- libtorrent-rasterbar: [Here](http://libtorrent.org/reference.html) and [here](http://libtorrent.org/manual.html).

### Branches


## Goals
1. Support multiple GUIs (Qt, Gtk, Cocoa, node-webkit)
2. RSS Automation
3. ......

- [0.1.0](https://github.com/gtorrent/gTorrent/issues/41)
- [0.0.1](https://github.com/gtorrent/gTorrent/issues/16)

## Dependencies

- cmake (make)
- git (make)
- boost
- glibmm
- gtkmm
- libtorrent-rasterbar

## Compiling/Installing
### GNU/Linux or Mac OS X
```
$ mkdir -p build/ && cd build/
$ cmake ..
$ make
```

#### Ubuntu notes
The version of libgtkmm in 14.04 is a bit out of date. To fix this do:

```
$ sudo add-apt-repository ppa:gnome3-team/gnome3-staging
$ sudo apt-get update
$ sudo apt-get dist-upgrade
```

#### ArchLinux
You have it easier, dumbass nerds and your "bleeding edge" repos.
Install from the [AUR](https://aur.archlinux.org/packages/gtorrent/)

### Windows
Refer [here](https://github.com/gtorrent/gTorrent/wiki/Building-on-Windows)
