#ifndef QTREE_H
#define QTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "image.h"

#define INFO(...) do {fprintf(stderr, "[          ] [ INFO ] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); fflush(stderr);} while(0)
#define ERROR(...) do {fprintf(stderr, "[          ] [ ERR  ] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); fflush(stderr);} while(0) 

typedef struct QTNode {
    unsigned char intensity;
    unsigned int row;
    unsigned int height;
    unsigned int column;
    unsigned int width;
    struct QTNode *children[4];

} QTNode;

QTNode *create_quadtree(Image *image, double max_rmse);  
QTNode *get_child1(QTNode *node);
QTNode *get_child2(QTNode *node);
QTNode *get_child3(QTNode *node);
QTNode *get_child4(QTNode *node);
unsigned char get_node_intensity(QTNode *node);
void delete_quadtree(QTNode *root);
void save_qtree_as_ppm(QTNode *root, char *filename); 
void save_preorder_qt(QTNode *root, char *filename); 

double calc_avrg_intensity(Image *image, int row, int col, int height, int width);
int split(Image *image, double max_rmse, int row, int col, int height, int width);
QTNode * create_children(Image *image, double max_rmse, int row, int col, int height, int width);
void save_preorder_helper(QTNode *, FILE *);
unsigned char recursive_find_intensity(QTNode *node, int i, int j);

#endif // QTREE_H