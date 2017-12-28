all:libtipwc.a libtipwc.so

libtipwc.a:ini.o ini_data.o ini_error.o
	mkdir -p include/tipwc lib
	cp *.h include/tipwc
	ar rcs libtipwc.a ini.o ini_data.o ini_error.o
	cp libtipwc.a lib/

libtipwc.so:ini.o ini_data.o ini_error.o
	gcc -O2 -shared -fPIC -o libtipwc.so ini.o ini_data.o ini_error.o -lsbuf
	cp libtipwc.so lib/

ini.o:ini.c ini.h ini_data.h
	gcc -O2 -c ini.c -shared -fPIC

ini_data.o:ini_data.h ini_data.c ini_error.h
	gcc -O2 -c ini_data.c -shared -fPIC

ini_error.o:ini_error.h ini_error.c
	gcc -O2 -c ini_error.c -shared -fPIC

clean:
	rm -rfv include lib *.a *.o *.so

install:
	cp -r include/tipwc /usr/include/ 
	cp lib/libtipwc.a lib/libtipwc.so /usr/lib/ 

uninstall:
	rm -rfv /usr/include/tipwc 
	rm /usr/lib/libtipwc.a /usr/lib/libtipwc.so
