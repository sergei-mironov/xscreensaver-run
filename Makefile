PREFIX=/usr/local

.PHONY: all install clean
all: xscreensaver-run

xscreensaver-run: src/xscreensaver-run.c
	gcc -lX11 $< -o $@

install: xscreensaver-run
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install $< $(DESTDIR)$(PREFIX)/bin/xscreensaver-run

clean:
	-rm xscreensaver-run

