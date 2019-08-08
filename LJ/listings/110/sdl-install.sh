#!/bin/bash
here=`pwd`
if [ `whoami` == "root" ]; then
    wget -cv http://www.libsdl.org/release/SDL-1.2.5.tar.gz
    wget -cv http://www.libsdl.org/projects/SDL_image/release/SDL_image-1.2.2.tar.gz
    wget -cv http://www.libsdl.org/projects/SDL_net/release/SDL_net-1.2.4.tar.gz
    wget -cv http://www.libsdl.org/projects/SDL_mixer/release/SDL_mixer-1.2.4.tar.gz
    wget -cv http://www.libsdl.org/projects/SDL_ttf/release/SDL_ttf-2.0.5.tar.gz

    rm -rf SDL-1.2.5
    rm -rf SDL_image-1.2.2
    rm -rf SDL_net-1.2.4
    rm -rf SDL_mixer-1.2.3
    rm -rf SDL_ttf-2.0.5

    tar -xzvf SDL-1.2.5.tar.gz
    tar -xzvf SDL_image-1.2.2.tar.gz
    tar -xzvf SDL_net-1.2.4.tar.gz
    tar -xzvf SDL_mixer-1.2.3.tar.gz
    tar -xzvf SDL_ttf-2.0.5.tar.gz

    cd SDL-1.2.5
#
# or use "./configure --prefix /usr" to put it where you want it
#
   ./configure
    make
    make install
    cd $here

    cd SDL_image-1.2.2
    ./configure
    make
    make install
    cd $here

    cd SDL_mixer-1.2.3
    ./configure
    make
    make install
    cd $here

    cd SDL_net-1.2.4
    ./configure
    make
    make install
    cd $here

    cd SDL_ttf-2.0.5
    ./configure
    make
    make install
    cd $here

else

    echo you must be ROOT to run this script

fi
