overlay: overlay.c
	gcc overlay.c -o overlay -lm `pkg-config --cflags --libs cairo x11 xfixes xcomposite)`
