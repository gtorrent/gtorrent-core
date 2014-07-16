#!/bin/bash
SATISFIED=0
if [ -f "/etc/debian_version" ]; then
  sudo apt-get install -y cmake libgtkmm-3.0-dev libglibmm-2.4-dev libtorrent-rasterbar-dev libboost-all-dev libssl-dev
  SATISFIED=1
fi

if [ -f "/etc/gentoo-release" ]; then
  emerge -vq dev-util/cmake dev-cpp/gtkmm dev-cpp/glibmm net-libs/rb_libtorrent dev-libs/boost dev-libs/openssl
  SATISFIED=1
fi

if [ "$SATISFIED" -ne "1" ]; then
  echo "Your system isn't supported yet."
fi
