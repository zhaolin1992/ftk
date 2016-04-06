# Introduction #

Build FTK under ubuntu 10.04

# Details #

  1. apt-get install automake
  1. apt-get install autoconf
  1. apt-get install libtool
  1. apt-get install libpng-dev
  1. apt-get install libjpeg-dev
  1. apt-get install libx11-dev
  1. apt-get install libXext-dev
  1. apt-get install gtk-doc-tools

if you got a problem like this:"warning: macro `AM\_GLIB\_GNU\_GETTEXT' not found in library"

> please :# apt-get install libglib2.0-dev
but you should do "make distclean; ./autogen.sh" before you do "./configure..." again