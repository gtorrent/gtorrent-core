#gTorrent
[![Build Status](https://travis-ci.org/gtorrent/gTorrent.svg?branch=master)](https://travis-ci.org/gtorrent/gTorrent)

[Screencast (old version)](http://a.pomf.se/lkowau.webm)

## About
A minimalistic lightweight bittorrent client written by some smart people from the 4chan technology board.

### Goals
 - Support multiple GUIs (Qt, Gtk, Cocoa, node-webkit)
 - RSS Automation
 - ......

#### Version specific
- [0.1.0](https://github.com/gtorrent/gTorrent/issues/41)
- [0.0.1](https://github.com/gtorrent/gTorrent/issues/16)

## Dependencies

- cmake (make)
- git (make)
- boost
- glibmm
- gtkmm
- libtorrent-rasterbar

## CONTRIBUTING
### How to contribute
#### Making changes
**As a non collaborator**
 1. Fork this repository
 2. Make a **new** branch
 3. Make your changes
 4. Make a pull request

**As a collaborator**
 1. Make a **new branch** on this repository
 2. Make your changes
 3. Make a pull request, and **leave it alone**.
 4. (optional) label it "PR: Merging" or "PR: Ready"

When someone decides we have enough PRs and is **willing** to go through the fucking merge hell:
 1. Make a **new** branch on your local machines.
 2. Pull each pr **one by one** into your new branch.
 3. Merge as necessary.
 4. Record each change into the file `changelog`
 5. Update the version in `cmakelists.txt`

#### Standards
##### Indentation
We are using tabs for indentations. Don't like it? I don't either, but it's our *standard* so please fucking follow it.
Fortunately, we are allowed to use spaces to align code. Will this cause a mess aligning using different tab sizes? *Yes.* So let's all
agree to use tabsize=8, alright? I didn't decide this one.

gTorrent uses astyle to format our code. You can find more about it [here](http://astyle.sourceforge.net/)

Install astyle, and then run scripts/format.sh at the root of the repo, so that someone else doesn't have to fix your retarded indents

##### Coding style
We are using our own, loosely based of kernel style. If something's not specified here, either follow what others have done or use your better judgement
- If/else statements get their opening braces (Don't ask why, I hate this one too.)
- Functions get their own opening braces.
- Avoid stupid fucking sepples shitty meta programming templates whenever possible.

### References to docmentations
Links to the documentation of each library.
- [gtkmm](https://developer.gnome.org/gtkmm/3.12/)
- libtorrent-rasterbar: [Here](http://libtorrent.org/reference.html) and [here](http://libtorrent.org/manual.html).

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
