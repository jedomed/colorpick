# colorpick
A simple color picker app I cobbled together in about 8 hours because none of the existing ones (gcolor2, gcolor3, KColorChooser, GPick, ...) support transparency and I just want something that'll quickly give me a RGB(A) + HEX code. This is my first time making a desktop app in Linux so I'm sure I did everything wrong. This is primarily for future me.

I'm pretty sure there's somemething wrong with the HSV > RGB calculations (changing saturation/value also changes hue, trying to change hue when the saturation/value is too low just breaks it), so until I figure that out I added a switch so it won't affect the RGB values. The RGBA calculations should be accurate though.

There's also a tiny Cinnamon applet that goes in */home/USER/.local/share/cinnamon/applets/*, just make sure to change the paths in the *applet.js* file and in the *colorpick_on_top* file. *colorpick_on_top* starts the app with "Always on Top" enabled using *wmctrl*.

#### Compiling:

	glib-compile-resources --target main_compiled.c main.xml --generate-source
	# take the whole main.c file and append it at the end of main_compiled.c
	gcc -o colorpick main_compiled.c -Wall `pkg-config --cflags --libs gtk+-3.0`
	-export-dynamic -lm
	./colorpick
