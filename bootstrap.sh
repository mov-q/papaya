#!/bin/sh

#libtoolize || glibtoolize
aclocal
automake --foreign --add-missing
autoconf

