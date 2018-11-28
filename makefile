all: build/.xmake/bin/xmake .null
	build/.xmake/bin/xmake

clean: build/.xmake/bin/xmake .null
	build/.xmake/bin/xmake clean

distclean: build/.xmake/bin/xmake .null
	build/.xmake/bin/xmake clean --all

rebuild: build/.xmake/bin/xmake .null
	build/.xmake/bin/xmake --rebuild

install: build/.xmake/bin/xmake .null
	build/.xmake/bin/xmake install

uninstall: build/.xmake/bin/xmake .null
	build/.xmake/bin/xmake uninstall

build/.xmake/bin/xmake: 
	if [ ! -d build/.xmake ]; then mkdir -p build/.xmake; fi
	if [ ! -f build/.xmake/xmake.zip ]; then wget https://github.com/tboox/xmake/archive/master.zip -O build/.xmake/xmake.zip; fi
	if [ ! -f build/.xmake/xmake.zip ]; then wget https://gitlab.com/tboox/xmake/-/archive/master/xmake-master.zip -O build/.xmake/xmake.zip; fi
	cd build/.xmake; unzip xmake.zip; cd xmake-master; ./install `pwd`/..

.PHONY:all clean distclean rebuild install uninstall .null
.null:

