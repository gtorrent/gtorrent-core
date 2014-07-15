# glibmm
wget http://ftp.gnome.org/pub/GNOME/sources/glibmm/2.40/glibmm-2.40.0.tar.xz
tar xf glibmm-2.40.0.tar.xz
cd glibmm-2.40.0
./configure
make
sudo make install
cd ..

# gtk+
wget http://ftp.gnome.org/pub/gnome/sources/gtk+/3.12/gtk+-3.12.2.tar.xz
tar xf gtk+-3.12.2.tar.xz
cd gtk+-3.12.2
./configure
make
sudo make install
cd ..

# gtkmm
wget http://ftp.gnome.org/pub/GNOME/sources/gtkmm/3.12/gtkmm-3.12.0.tar.xz
tar xf gtkmm-3.12.0.tar.xz
cd gtkmm-3.12.0
./configure
make
sudo make install
cd ..

