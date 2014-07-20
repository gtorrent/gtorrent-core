#gTorrent
[![Build Status](https://travis-ci.org/gtorrent/gTorrent.svg?branch=master)](https://travis-ci.org/gtorrent/gTorrent)
[Screencast (old version)](http://a.pomf.se/lkowau.webm)

## CONTRIBUTING
### HOW TO CONTRIBUTE
#### Making changes (**PLEASE READ**)
**AS A NON COLLABORATOR**
1. FORK THIS REPOSITORY
2. MAKE A **NEW** BRANCH
3. MAKE YOUR CHANGES
4. MAKE A PULL REQUEST

**AS A COLLABORATOR**
1. MAKE A **NEW BRANCH** ON THIS REPOSITORY
2. MAKE YOUR CHANGES
3. MAKE A PULL REQUEST, AND **LEAVE IT ALONE**.
4. (OPTIONAL) LABEL IT "PR:MERGING" OR "PR:READY" FAGGOTS.

When someone decides we have enough PRs and is **willing** to go through the fucking merge hell:
1. MAKE A **NEW** branch on your local machines
2. PULL EACH PR **ONE BY ONE** INTO YOUR NEW BRANCH
3. MERGE AS NECESSARY
4. RECORD EACH CHANGE INTO THE FILE `CHANGELOG`
5. UPDATE THE VERSION IN `CMAKELISTS.TXT`

#### Standards
##### Indentation
We are using tabs for indentations. Don't like it? I don't either, but it's our *standard* so please fucking follow it.
Fortunately, we are allowed to use spaces to align code. Will this cause a mess aligning using different tab sizes? *Yes.* So let's all
agree to use tabsize=8, alright? I didn't decide this one.

gTorrent uses astyle to format our code. You can find more about it [here](http://astyle.sourceforge.net/)

Install astyle, and then run scripts/format.sh at the root of the repo, so that someone else doesn't have to fix your retarded indents

##### Coding style
We are using our own, loosely based of kernel style. If something's not specified here, either follow what others have done or use your better judgement
- if/else statements get their opening braces (Don't ask why, I hate this one too.)
- functions get their own opening braces.
- avoid stupid fucking sepples shitty meta programming templates whenever possible.

### References to docmentations
Links to the documentation of each library.
- [gtkmm](https://developer.gnome.org/gtkmm/3.12/)
- libtorrent-rasterbar: [Here](http://libtorrent.org/reference.html) and [here](http://libtorrent.org/manual.html).

## About
A minimalistic lightweight bittorrent client written by some smart people from the 4chan technology board.

### Goals
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
