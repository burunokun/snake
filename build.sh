#!/bin/sh

set -xe

export PKG_CONFIG_PATH=`find ~ -type d -name \*pkgconfig\* | grep sdl`
CFLAGS="-Wall -Wextra -Werror -ggdb"
SDL=`pkg-config --cflags --libs sdl2`

gcc $CFLAGS -o snake src/snake.c $SDL

