
Debian
====================
This directory contains files used to package gkcd/gkc-qt
for Debian-based Linux systems. If you compile gkcd/gkc-qt yourself, there are some useful files here.

## gkc: URI support ##


gkc-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install gkc-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your gkcqt binary to `/usr/bin`
and the `../../share/pixmaps/gkc128.png` to `/usr/share/pixmaps`

gkc-qt.protocol (KDE)

