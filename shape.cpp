
/* 
 * File:   shape.h
 * Author: Sahil Gupta
 *
 * Created on April 27, 2013, 6:45 PM
 */
#include "shape.h"

int Roundoff(float num) {
    return (int) (num + 0.5);
}

float convertDToRad(float degrees) {
    float PI = atan(1)*4;
    return (degrees / 180)*PI;
}

float convertRadToD(float radians) {
    float PI = atan(1)*4;
    return (radians / PI)*180;
}

void shape::setLocation(float x, float y, float theta) {
    this->x = x;
    this->y = y;
    this->theta = theta;
}

bool shape::isPointInside(int x, int y, int ImageWidth, int ImageHeight) {
    if ((x >= 0 && y >= 0 && x < ImageWidth && y < ImageHeight))
        return true;
    else
        return false;
}

void shape::drawOutline(unsigned char *image, int ImageWidth, int ImageHeight) {
    int i = (int) (this->theta) / 10;
    int x, y;
    int Size = lutSizes[i];
    int j;
    for (j = 0; j < Size; j += 2) {
        x = *(*(lut + i) + j) + (this->x);
        y = *(*(lut + i) + (j + 1)) + (this->y);
        image[OFFSET(Roundoff(y), Roundoff(x), ImageWidth)] = 80;
    }
}

bool shape::isValidLocation(unsigned char *image, int ImageWidth, int ImageHeight) {
    int i = (int) (this->theta) / 10;
    int x, y;
    int Size = lutSizes[i];
    int j;
    for (j = 0; j < Size; j += 2) {
        x = *(*(lut + i) + j) + (this->x);
        y = *(*(lut + i) + (j + 1)) + (this->y);
        if (!isPointInside(Roundoff(x), Roundoff(y), ImageWidth, ImageHeight) ||
                image[OFFSET(Roundoff(y), Roundoff(x), ImageWidth)] == 0) {
            return false;
        }
    }
}
