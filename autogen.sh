#!/bin/sh

echo "Generating build information using aclocal, autoheader, automake and autoconf"
echo "This may take a while ..."

# Regenerate configuration files.

if ! grep -q printer.cpp ./src/hardware/Makefile.am; then
	echo "Add syncgw extension patches - part#1..."
	cat ./src/hardware/Makefile.am | sed --expression="s/ipx.cpp /ipx.cpp printer.cpp /g" >./src/hardware/Makefile.tmp
	cp ./src/hardware/Makefile.tmp ./src/hardware/Makefile.am
fi

aclocal
autoheader
automake --include-deps --add-missing --copy 
autoconf

if [ -e ./config.h.in~ ]; then
	rm ./config.h.in~
fi
if ! grep -q syncgw ./config.h.in; then
	echo "Add syncgw extension patches - part#2..."
	echo "#include \"syncgw.h\"" >> ./config.h.in
fi

echo "Now you are ready to run ./configure."
echo "You can also run  ./configure --help for extra features to enable/disable."
