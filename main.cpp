#include <stdlib.h>      //Included for function exit()
#include <fstream>       //Included for ifstream class
#include <iostream>      //Included for ostream class (cout)
#include "shape.h"

using namespace std;

inline float rint(float x) {return float((int)(x+.5));}

#define D_THETA  10    //Angle increment (degrees) between neighboring z-indeces

//Delcaration of functions to read/write pgm images
//-------------------------------------------------

int readPGM(const char *filename,unsigned char *&image,int &xsize,int &ysize);
int writePGM(const char *filename,unsigned char *image,int xsize,int ysize);

//Declaration of function to compute "distance to destination" shape in 3D grid
//----------------------------------------------------------------------------
//obst = binary input obstacle array (xsize by ysize)
//dist = output array of distances (xsize by ysize by zsize)
// s   = shape object to be navigated through the obstacles
//dest = grid index of destination location (where dist = 0)

void computeDistances(
 unsigned char *obst,shape *s,int dest,int *dist,int xsize,int ysize,int zsize
);

//Read rhombus parameters (width,height,angle) from file and pass them to object

void readAndSetRhombusParams(ifstream &fin,rhombus *myrhom) {
  float width,height,corner_angle;
  fin >> width;        if (!fin.good()) {cout << "Invalid width\n";  exit(1);}
  fin >> height;       if (!fin.good()) {cout << "Invalid height\n"; exit(1);}
  fin >> corner_angle; if (!fin.good()) {cout << "Invalid angle\n";  exit(1);}
  myrhom->setParams(width,height,corner_angle);
}

//Read lens parameters (radius,arc_angle) from file and pass them to object

void readAndSetLensParams(ifstream &fin,lens *mylens) {
  float radius, arc_angle;
  fin >> radius;    if (!fin.good()) {cout << "Invalid radius\n";    exit(1);}
  fin >> arc_angle; if (!fin.good()) {cout << "Invalid arc_angle\n"; exit(1);}
  mylens->setParams(radius,arc_angle);
}

//Read capsule parameters (width,height) from file and pass them to object

void readAndSetCapsuleParams(ifstream &fin,capsule *mycaps) {
  float width,height;
  fin >> width;  if (!fin.good()) {cout << "Invalid width\n";  exit(1);}
  fin >> height; if (!fin.good()) {cout << "Invalid height\n"; exit(1);}
  mycaps->setParams(width,height);
}

//Read starting/destination location from file and return nearest grid point

int readAndSetLocation(ifstream &fin,shape *myshape,int xsize,int ysize) {
  float x,y,theta;
  fin >> x;     if (!fin.good()) {cout << "invalid x-coordinate\n"; exit(1);}
  fin >> y;     if (!fin.good()) {cout << "invalid y-coordinate\n"; exit(1);}
  fin >> theta; if (!fin.good()) {cout << "invalid rotation angle\n"; exit(1);}
  while (theta<0   ) theta+=360;             //Represent theta angle
  while (theta>=360) theta-=360;             //between 0 and 360 deg.
  x=rint(x); y=rint(y); theta=rint(theta/D_THETA);
  myshape->setLocation(x,y,theta*D_THETA);
  return int(x+xsize*(y+ysize*theta));
}


int main(int argc,char *argv[]) {

  //Make sure number of command line arguments is correct

  if (argc<=3) {
    cout << "Missing arguments. Should be: "<< argv[0]
         << " obstacle.pgm output.pgm shape.txt" << endl;
    exit(1);
  }

  //Read input image (obstacles) and record grid dimensions

  unsigned char *image;
  int xsize, ysize, zsize=360/D_THETA;

  if (!readPGM(argv[1],image,xsize,ysize)) {
    cout << "Error in reading input image: " << argv[1] << endl;
    exit(1);
  }

  int gridsize=xsize*ysize*zsize;     //Total number of pixels in the grid

  //Open shape input file

  ifstream fin;
  fin.open(argv[3]);

  if (!fin.is_open()) {
    cout << "Could not open shape file: " << argv[3] << endl;
    exit(1);
  }

  //Read from shape input file (single line)

  char shapetype;
  fin >> shapetype;                    //Read first character from shape file

  //Get shape parameters from input file

  rhombus myrhom; lens mylens; capsule mycaps;        //Derived shape objects
  shape *myshape;  //Base pointer to one of the above 3 derived shape objects

  switch (shapetype) {
    case 'R': myshape=&myrhom; readAndSetRhombusParams(fin,&myrhom); break;
    case 'L': myshape=&mylens; readAndSetLensParams(fin,&mylens);    break;
    case 'C': myshape=&mycaps; readAndSetCapsuleParams(fin,&mycaps); break;
    default:  cout << "Unrecognized shape type\n"; exit(1);
  }

  //Get starting location and draw starting shape outline

  int p1=readAndSetLocation(fin,myshape,xsize,ysize); //Starting point

  if (myshape->isValidLocation(image,xsize,ysize)) {
    myshape->drawOutline(image,xsize,ysize);      //Draw starting shape
  } else {
    cout << "Illegal start location." << endl;
    exit(1);
  }

  //Get destination location and draw starting shape outline

  int p2=readAndSetLocation(fin,myshape,xsize,ysize); //Destination point

  if (myshape->isValidLocation(image,xsize,ysize)) {
    myshape->drawOutline(image,xsize,ysize);      //Draw destination shape
  } else {
    cout << "Illegal destination." << endl;
    exit(1);
  }

  //Close the shape input file

  fin.close();

  //Compute "distance to destination" function at valid (x,y,theta) locations

  int *dist=new int[gridsize];                     //Allocate array for dist
  for (int p=0; p<gridsize; p++) dist[p]=gridsize; //Initialize dist values

  computeDistances(image,myshape,p2,dist,xsize,ysize,zsize);

  //Move shape from starting point to destination via the optimal path

  int steps=0;
  int X=1, Y=xsize, Z=xsize*ysize;    //Offsets needed to move by one pixel
  
  p2=p1;     //Initialize neighbor point p2 to the starting point p1

  do {
    p1=p2;                                   //Move p1 to smallest neighbor p2
    int k=p1/Z, j=(p1-k*Z)/Y, i=p1-k*Z-j*Y;  //3D indeces of current point

    //Periodically draw shape (every 15 time steps) at the current point p1

    if (steps++==15) {
      myshape->setLocation(i,j,k*D_THETA);
      myshape->drawOutline(image,xsize,ysize);
      steps=0;
    }

    //Possibly move in x-direction

    if (i>0       && dist[p2-X]<dist[p2]) p2-=X; else
    if (i<xsize-1 && dist[p2+X]<dist[p2]) p2+=X;

    //Possibly move in y-direction

    if (j>0       && dist[p2-Y]<dist[p2]) p2-=Y; else
    if (j<ysize-1 && dist[p2+Y]<dist[p2]) p2+=Y;

    //Possibly move in z-direction (rotation by DTHETA)

    if (k>0        && dist[p2-Z]         <dist[p2]) p2-=Z;          else
    if (k<zsize-1  && dist[p2+Z]         <dist[p2]) p2+=Z;          else
    if (k==0       && dist[p2-Z+gridsize]<dist[p2]) p2+=gridsize-Z; else
    if (k==zsize-1 && dist[p2+Z-gridsize]<dist[p2]) p2-=gridsize-Z;

  } while (p1!=p2);

  //Write output image

  if (!writePGM(argv[2],image,xsize,ysize))
    cout << "Unable to write output image: " << argv[2] << endl;

  //Deallocate memory

  delete[] image;
  delete[] dist;

  return 0;
}
