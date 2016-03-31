/* 
 * File:   region.h
 * Author: Sahil Gupta
 * Date Submitted: 4/8/2012
 */

#include <stdio.h>
#include <string.h>

int readPGM(const char *filename,unsigned char *&image,int &xsize,int &ysize) {
  FILE *file;
  char line[81],word[81];

  //Initialize image sizes and pointer

  image=NULL; xsize=ysize=0;

  //Open input file for reading

  if (!(file=fopen(filename,"r"))) return 0;

  //Read PGM header

  fgets(line,81,file);                              //Read first header line
  sscanf(line,"%s",word);                           //Ingore trailing whitespace

  if (strcmp(word,"P5")) {fclose(file); return 0;}  //Check for PGM identifier

  do fgets(line,81,file); while (*line=='#');       //Read past comments

  sscanf(line,"%d %d",&xsize,&ysize);               //Read image size

  fgets(line,81,file);                              //Read past last header line

  //Allocate space for image data

  if (xsize<=0 || ysize<=0) {fclose(file); xsize=ysize=0; return 0;}

  image=new unsigned char[xsize*ysize];

  //Read image data from file

  bool good= fread(image,sizeof(unsigned char),xsize*ysize,file) == xsize*ysize;
  if (!good) {delete[] image; image=NULL; xsize=ysize=0;}

  //Close file and return

  fclose(file);
  return good;
}


int writePGM(const char *filename,unsigned char *image,int xsize,int ysize) {
  FILE *file;

  //Open output file for writing

  if (!(file=fopen(filename,"w"))) return 0;

  //Write PGM header

  fprintf(file,"P5\n");
  fprintf(file,"# CREATOR: pgmio.cpp  Rev: 3/21/10\n");
  fprintf(file,"# GENERATOR: writePGM\n");
  fprintf(file,"%d %d\n",xsize,ysize);
  fprintf(file,"255\n");

  //Write image data to file

  fwrite(image,sizeof(unsigned char),xsize*ysize,file);

  //Close file and return

  fclose(file);
  return 1;
}
