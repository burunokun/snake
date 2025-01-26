#!/bin/sh

set -xe

export PKG_CONFIG_PATH=`find ~ -type d -name \*pkgconfig\* | grep sdl`
gcc -Wall -Wextra -Werror -o snake snake.c $(pkg-config --cflags --libs sdl2)
