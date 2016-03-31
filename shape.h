/* 
 * File:   shape.h
 * Author: Sahil Gupta
 *
 * Created on April 27, 2013, 6:45 PM
 */

#ifndef SHAPE_H
#define SHAPE_H

#include <math.h>

#define OFFSET(r, c, numcols) (r * numcols + c)
#define LUTSIZE 36
#define DEGINCREMENT 10

using namespace std;

int Roundoff(float num);
float convertDToRad(float degrees);
float convertRadToD(float radians);

class shape {
protected:
    float x; /*x location of the shape "reference origin" and rotation pivot point*/
    float y; /*y location of the shape "reference origin" and rotation pivot point*/
    float theta; /*angle by which shape has been rotated around its pivot point*/
    int **lut; /*look up table stores the pre-calculated values of the 36 rotations possible*/
    int lutSizes[LUTSIZE];
    /*Below are the four dimensions of the boundary box possible*/
    int maxXLut;
    int minXLut;
    int maxYLut;
    int minYLut;

public:

    void setLocation(float x, float y, float theta);
    bool isPointInside(int x, int y, int ImageWidth, int ImageHeight);
    void drawOutline(unsigned char *image, int ImageWidth, int ImageHeight);
    bool isValidLocation(unsigned char *image, int ImageWidth, int ImageHeight);
    void printLutData();
    void printLutSizes();
};

class line {
protected:
    float x;
    float y;
    float length; /*length of the line, radius for the arc*/
    float N; /*sample points of the line*/
    float theta; /*angle of rotation the line*/

public:

    /**
     * Constructor for the line which can be a line/arc
     * @param length Length of the line or radius of the arc
     * @param x Starting x coordinate
     * @param y Starting y coordinate
     * @param theta Theta value in radians
     */
    line(float length, float x, float y, float theta) :
    x(x), y(y), length(length), theta(theta) {
    }
};

class arc : public line {
private:
    float lengthArc; /*length of the arc*/
    float arcAngle; /*arc angle in radians*/
    float radian_increment; /*x increment of the x coordinate line-segment*/
    float radians;

public:

    /**
     * Constructor of the arc.
     * @param length Radius of the arc in radians
     * @param x Starting x coordinate
     * @param y Starting y coordinate
     * @param theta Theta value in radians
     * @param arcAngle Arc angle in radians
     */

    arc(float length, float x, float y, float theta, float arcAngle) : line(length, x, y, theta) {
        this->arcAngle = arcAngle;
        this->lengthArc = this->arcAngle * this->length;
        this->N = 5 * this->lengthArc;
        this->radian_increment = this->arcAngle / this->N;
        this->radians = theta;
    }

    void ComputeLut(int *lut, int *lutSize) {
        int tempN = Roundoff(N);
        int i;
        float x_plot, y_plot;

        x_plot = this->x + (length * cos(radians));
        y_plot = this->y + (length * sin(radians));
        lut[(*lutSize)++] = Roundoff(x_plot);
        lut[(*lutSize)++] = Roundoff(y_plot);

        for (i = 1; i <= tempN; i++) {
            x_plot = this->x + (length * cos(radians));
            y_plot = this->y + (length * sin(radians));
            if (((Roundoff(y_plot) != lut[(*lutSize) - 1]) || (Roundoff(x_plot) != lut[(*lutSize) - 2]))) {
                lut[(*lutSize)++] = Roundoff(x_plot);
                lut[(*lutSize)++] = Roundoff(y_plot);
            }
            radians += radian_increment;
        }
    }

};

class linesegment : public line {
private:
    float x_increment; /*x increment of the x coordinate line-segment*/
    float y_increment; /*y increment of the y coordinate line-segment*/

public:

    /**
     * Constructor for the line segment
     * @param length Length of the line segment
     * @param x Starting x coordinate
     * @param y Starting y coordinate
     * @param theta Theta value in radians
     */
    linesegment(float length, float x, float y, float theta) : line(length, x, y, theta) {
        this->N = this->length * 5;
        this->x_increment = (this->length * cos(this->theta)) / N;
        this->y_increment = (this->length * sin(this->theta)) / N;
    }

    void ComputeLut(int *lut, int *lutSize) {
        int tempN = Roundoff(N);
        int i;
        float x_plot = this->x;
        float y_plot = this->y;
        lut[(*lutSize)++] = Roundoff(x_plot);
        lut[(*lutSize)++] = Roundoff(y_plot);

        for (i = 1; i <= tempN; i++) {
            x_plot += x_increment;
            y_plot += y_increment;
            if ((Roundoff(y_plot) != lut[(*lutSize) - 1]) || (Roundoff(x_plot) != lut[(*lutSize) - 2])) {
                lut[(*lutSize)++] = Roundoff(x_plot);
                lut[(*lutSize)++] = Roundoff(y_plot);
            }
        }
    }
};

class rhombus : public shape {
private:
    float width;
    float height;
    float corner_angle;

public:

    rhombus() {
        this->x = 0.0;
        this->y = 0.0;
        this->theta = 0.0;
    }

    void setParams(float width, float height, float corner_angle) {
        this->width = width;
        this->height = height;
        this->corner_angle = convertDToRad(corner_angle);
        float x = this->x;
        float y = this->y;
        float W = this->width;
        float H = this->height;
        float PI = atan(1)*4;
        this->lut = new int*[LUTSIZE];
        int i;
        int tempImageSize = (int) ((width + height) * 20);
        for (i = 0; i < LUTSIZE; i++) {
            lut[i] = new int[tempImageSize];
            lutSizes[i] = 0;
        }

        int j;
        float newTheta;
        for (j = 0; j < LUTSIZE; j++) {
            newTheta = j * 10;
            newTheta = convertDToRad(newTheta);

            float x1 = x;
            float y1 = y;
            float length1 = W;
            float theta1 = newTheta;
            linesegment l1(length1, x1, y1, theta1);
            l1.ComputeLut(this->lut[j], (lutSizes + j));

            float x2 = x1 + (W * cos(theta1));
            float y2 = y1 + (W * sin(theta1));
            float length2 = H;
            float theta2 = newTheta + this->corner_angle;
            linesegment l2(length2, x2, y2, theta2);
            l2.ComputeLut(this->lut[j], (lutSizes + j));

            float x3 = x2 + (H * cos(theta2));
            float y3 = y2 + (H * sin(theta2));
            float length3 = W;
            float theta3 = newTheta + PI;
            linesegment l3(length3, x3, y3, theta3);
            l3.ComputeLut(this->lut[j], (lutSizes + j));

            float x4 = x3 + (W * cos(theta3));
            float y4 = y3 + (W * sin(theta3));
            float length4 = H;
            float theta4 = newTheta + PI + this->corner_angle;
            linesegment l4(length4, x4, y4, theta4);
            l4.ComputeLut(this->lut[j], (lutSizes + j));
        }
    }
};

class lens : public shape {
private:
    float arcRadius;
    float arc_angle;

public:

    lens() {
        this->x = 0.0;
        this->y = 0.0;
        this->theta = 0.0;
    }

    void setParams(float arcRadius, int arc_angle) {
        this->arcRadius = arcRadius;
        this->arc_angle = convertDToRad(arc_angle);

        float PI = atan(1)*4;
        float x = this->x;
        float y = this->y;
        float separation = 2 * this->arcRadius * cos(this->arc_angle / 2);

        this->lut = new int*[LUTSIZE];
        int i;
        int tempArcLength = this->arc_angle * (2 * PI * this->arcRadius);

        int tempImageSize = (int) (tempArcLength * 20);
        for (i = 0; i < LUTSIZE; i++) {
            lut[i] = new int[tempImageSize];
            lutSizes[i] = 0;
        }

        int j;
        float newTheta;
        for (j = 0; j < LUTSIZE; j++) {
            newTheta = j * 10;
            newTheta = convertDToRad(newTheta);

            float v_unit_x = cos(newTheta + (this->arc_angle / 2));
            float v_unit_y = sin(newTheta + (this->arc_angle / 2));
            float V_x = v_unit_x * separation;
            float V_y = v_unit_y * separation;

            float x1 = x;
            float y1 = y;
            float theta1 = newTheta;

            arc a1(this->arcRadius, x1, y1, theta1, this->arc_angle);
            a1.ComputeLut(this->lut[j], (lutSizes + j));

            float x2 = x + V_x;
            float y2 = y + V_y;
            float theta2 = newTheta + PI;

            arc a2(this->arcRadius, x2, y2, theta2, this->arc_angle);
            a2.ComputeLut(this->lut[j], (lutSizes + j));
        }
    }
};

class capsule : public shape {
private:
    float width;
    float height;

public:

    capsule() {
        this->x = 0.0;
        this->y = 0.0;
        this->theta = 0.0;
    }

    void setParams(float width, float height) {
        this->width = width;
        this->height = height;
        float PI = atan(1)*4;
        float x = this->x;
        float y = this->y;
        float W = this->width;
        float H = this->height;

        this->lut = new int*[LUTSIZE];
        int i;
        int tempArcLength = ((PI / 2) * (2 * PI * (H / 2))) + width;

        int tempImageSize = (int) (tempArcLength * 50);
        for (i = 0; i < LUTSIZE; i++) {
            lut[i] = new int[tempImageSize];
            lutSizes[i] = 0;
        }

        int j;
        float newTheta;
        for (j = 0; j < LUTSIZE; j++) {
            newTheta = j * 10;
            newTheta = convertDToRad(newTheta);

            float x1 = x;
            float y1 = y;
            float length1 = W;
            float theta1 = newTheta;
            linesegment l1(length1, x1, y1, theta1);
            l1.ComputeLut(this->lut[j], (lutSizes + j));

            float x2 = x + (W * cos(newTheta)) - (H * sin(newTheta));
            float y2 = y + (W * sin(newTheta)) + (H * cos(newTheta));
            float length2 = W;
            float theta2 = newTheta + PI;
            linesegment l2(length2, x2, y2, theta2);
            l2.ComputeLut(this->lut[j], (lutSizes + j));

            float x3 = x + (W * cos(newTheta)) - ((H / 2) * sin(newTheta));
            float y3 = y + (W * sin(newTheta)) + ((H / 2) * cos(newTheta));
            float theta3 = newTheta - (PI / 2);
            float radius3 = H / 2;
            float arc_angle3 = PI;

            arc a3(radius3, x3, y3, theta3, arc_angle3);
            a3.ComputeLut(this->lut[j], (lutSizes + j));

            float x4 = x - ((H / 2) * sin(newTheta));
            float y4 = y + ((H / 2) * cos(newTheta));
            float theta4 = newTheta + (PI / 2);
            float radius4 = H / 2;
            float arc_angle4 = PI;

            arc a4(radius4, x4, y4, theta4, arc_angle4);
            a4.ComputeLut(this->lut[j], (lutSizes + j));
        }
    }
};

template <class T> class list {
protected:

    int arraysize; //Maximum allowed size of list

    T *array; //Array used to hold list items
    T *current; //Pointer to next list item to be read (sequential access)
    T *tail; //Pointer to first empty item in list

public:

    //Contructors

    list() {
        array = current = tail = 0;
        arraysize = 0;
    }

    list(int maxsize) {
        array = 0;
        allocate(maxsize);
    }

    ~list() {
        deallocate();
    }

    //Memory allocation

    int allocate(int maxsize) {
        if (arraysize != maxsize) {
            deallocate();
            if (!(array = new T[maxsize])) return 0;
        }
        arraysize = maxsize;
        clear();
        return 1;
    }

    void deallocate() {
        delete[] array;
        array = current = tail = 0;
        arraysize = 0;
    }

    //Manipulating the list

    void clear() {
        current = tail = array;
    }

    void append(T item) {
        *tail++ = item;
    } //Does not check for overstepping array

    int getSize() {
        return tail - array;
    }

    //Sequential access

    void rewind() {
        current = array;
    }

    int getNext(T &item) {
        if (current != tail) {
            item = *current++;
            return 1;
        } else return 0;
    }

    void deleteNext() {
        if (current != tail) *current = *--tail;
    }

    void fforward() {
        current = tail;
    }

    int getPrev(T &item) {
        if (current != array) {
            item = *--current;
            return 1;
        } else return 0;
    }

};

#endif
