PREFIX = /usr/local/bin

sg2fcitx: sg2fcitx.o
	gcc mainme.c -o sg2fcitx
sg2fcitx.o: mainme.c
	gcc -Wall mainme.c -o sg2fcitx.o

clean:
	rm sg2fcitx.o
	rm sg2fcitx

install:
	cp sg2fcitx $(PREFIX)
	cp gbkpy.org $(PREFIX)

uninstall:
	rm $(PREFIX)/sg2fcitx
	rm $(PREFIX)/gbkpy.org
