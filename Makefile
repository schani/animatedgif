palette : palette.c
	gcc --std=c99 -Wall -DRWIMG_JPEG -DRWIMG_PNG -DRWIMG_GIF -o palette palette.c rwimg/readimage.c rwimg/rwgif.c rwimg/rwjpeg.c rwimg/rwpng.c -ljpeg -lpng -lgif
