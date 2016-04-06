# Introduction #

How to Build Ftk Under Fedora12




**Should install these Packages :**
> automake autoconf sysconftool libtool libjpeg-devel libpng-devel libX11-devel libXext-devel gtk-doc
**Use the following command to install these packages:**
> 'sudo yum install automake autoconf sysconftool libtool libjpeg-devel libpng-devel libX11-devel libXext-devel gtk-doc '
If you got a error like this:
configure.in:197: warning: macro `AM\_GLIB\_GNU\_GETTEXT' not found in library
please yum install glib2-devel.i686