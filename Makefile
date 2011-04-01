palette : palette.c dither.c writegif.c
	gcc -g --std=c99 -Wall -DRWIMG_JPEG -DRWIMG_PNG -DRWIMG_GIF -o palette palette.c dither.c writegif.c rwimg/readimage.c rwimg/writeimage.c rwimg/rwgif.c rwimg/rwjpeg.c rwimg/rwpng.c -ljpeg -lpng -lgif
