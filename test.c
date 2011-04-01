#include <stdlib.h>

#include "rwimg/readimage.h"
#include "rwimg/writeimage.h"

#include "writegif.h"

int
main (int argc, char *argv [])
{
	int i;
	gif_write_state_t *state = NULL;

	for (i = 1; i < argc; ++i) {
		int width, height;
		unsigned char *data = read_image (argv [i], &width, &height);
		if (state == NULL)
			state = gif_write_start ("out.gif", width, height, 3, 3, 2);
		gif_write_frame (state, data, width * 3, 3);
		free (data);
	}
	if (state != NULL)
		gif_write_close (state);
	return 0;
}
