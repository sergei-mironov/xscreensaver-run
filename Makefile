
.PHONY: all
all: xscreensaver-run

xscreensaver-run: src/xscreensaver-run.c
	gcc -lX11 $< -o $@
