#ifndef PNMDEFS_H
#define PNMDEFS_H

typedef struct pnm_ {
  unsigned char *data; /* The color values for each pixel. Each
			* value represents one pixel. Pixels are
			* represented, starting at the top left 
			* corner of the image, from left to right
			* and top to bottom. ie: data[x + width * y]
			* = pixel @ (x, y)
			*/
  int width;		/* Width of the image */
  int height;		/* Height of the image */
  int max_gray; 	/* Maximum color value for this image			*/
  int img_size; 	/* Size of image (Width * Height)*/
} pnm;

/*
  The following two functions for reading PGM files (PGM is raw-data PNM
  as opposed to ASCI-data PNM) are defined in the provided file pgmio.cpp
*/

int readPGM(const char *filename,unsigned char *&image, int &xsize, int &ysize);
int writePGM(const char *filename, unsigned char *image, int xsize, int ysize);

#endif
