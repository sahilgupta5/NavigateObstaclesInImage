#include <string.h>   //for memset()
#include "shape.h"

void computeDistances
(unsigned char *obst, shape *s, int seed, int *dist, int xsize, int ysize, int zsize)
//obst = binary input obstacle array (xsize by ysize)
//dist = output array of distances (xsize by ysize)
// s   = shape object to be navigated through the obstacles
//seed = grid index of destination location (where distance = 0)
{
    int p, q; //Index of current point and neighbor
    int i, j, k; //2D offsets of current point
    int gridsize = xsize * ysize*zsize; //Total size of grid

    const int X = 1; //Offset equivalent to 1 step in x directiond
    const int Y = xsize; //Offset equivalent to 1 step in y direction
    const int Z = xsize*ysize; //Offset equivalent to 1 step in z direction

    //Initialize visited flags and queue

    int *visited = new int[gridsize]; //Marks which points have been visited
    list<int> queue(gridsize); //Queue used to determine visiting order

    //Initialize arrays and queue

    memset(visited, 0, gridsize * sizeof (int));

    dist[seed] = 0;
    visited[seed] = 1;
    queue.append(seed);

    //Build distance function by marching outwards from seed

#define VALID(i,j,k) \
  (s->setLocation(i,j,(k * 10)), s->isValidLocation(obst,xsize,ysize))
    queue.rewind();
    while (queue.getNext(p)) {
        k = p / Z;
        j = (p - k * Z) / Y;
        i = (p - j*Y - k*Z); //Convert 1D index to 2D indeces

        if (i > 0 && !visited[q = p - X]) { //Visit -X neighbor
            visited[q] = 1;
            if (VALID(i - 1, j, k)) {
                dist[q] = dist[p] + 1;
                queue.append(q);
            }
        }

        if (i < xsize - 1 && !visited[q = p + X]) { //Visit +X neighbor
            visited[q] = 1;
            if (VALID(i + 1, j, k)) {
                dist[q] = dist[p] + 1;
                queue.append(q);
            }
        }

        if (j > 0 && !visited[q = p - Y]) { //Visit -Y neighbor
            visited[q] = 1;
            if (VALID(i, j - 1, k)) {
                dist[q] = dist[p] + 1;
                queue.append(q);
            }
        }

        if (j < ysize - 1 && !visited[q = p + Y]) { //Visit +Y neighbor
            visited[q] = 1;
            if (VALID(i, j + 1, k)) {
                dist[q] = dist[p] + 1;
                queue.append(q);
            }
        }

        if (k > 0 && !visited[q = p - Z]) { //Visit -Z neighbor
            visited[q] = 1;
            if (VALID(i, j, (k - 1))) {
                dist[q] = dist[p] + 1;
                queue.append(q);
            } else if (k == 0 && !visited[q = p - Z + gridsize]) { //WRAP AROUND
                visited[q] = 1;
                if (VALID(i, j, 35)) {
                    dist[q] = dist[p] + 1;
                    queue.append(q);
                }
            }
        }

        if (k < zsize - 1 && !visited[q = p + Z]) { //Visit +Z neighbor
            visited[q] = 1;
            if (VALID(i, j, (k + 1))) {
                dist[q] = dist[p] + 1;
                queue.append(q);
            } else if (k == (zsize - 1) && !visited[q = p + Z - gridsize]) { //WRAP AROUND
                visited[q] = 1;
                if (VALID(i, j, 0)) {
                    dist[q] = dist[p] + 1;
                    queue.append(q);
                }
            }
        }
    }

#undef VALID

    delete[] visited;
}
