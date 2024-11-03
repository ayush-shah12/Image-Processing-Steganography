#include "qtree.h"

QTNode *create_quadtree(Image *image, double max_rmse)
{
    QTNode *node = (QTNode *)malloc(sizeof(QTNode));
    node->intensity = calc_avrg_intensity(image, 0, 0, image->height, image->width);
    node->row = 0;
    node->column = 0;
    node->height = image->height;
    node->width = image->width;
    for (int i = 0; i < 4; i++)
    {
        node->children[i] = NULL;
    }

    if(node-> height == 1 && node->width == 1){
        return node;
    }

    if (node->height == 1)
    {
        if (split(image, max_rmse, node->row, node->column, node->height, node->width) == 1)
        {
            node->children[0] = create_children(image, max_rmse, node->row, node->column, node->height, node->width / 2);
            node->children[1] = create_children(image, max_rmse, node->row, node->column + node->width / 2, node->height, (node->width - node->width / 2));
            return node;
        }
    }

    else if (node->width == 1)
    {
        if (split(image, max_rmse, node->row, node->column, node->height, node->width) == 1)
        {
            node->children[0] = create_children(image, max_rmse, node->row, node->column, node->height / 2, node->width);
            node->children[2] = create_children(image, max_rmse, node->row + node->height / 2, node->column, (node->height - node->height / 2), node->width);
            return node;
        }
    }

    

    if (split(image, max_rmse, node->row, node->column, node->height, node->width) == 1)
    {
        int height1 = node->height / 2;
        int height2 = node->height - height1;
        int width1 = node->width / 2;
        int width2 = node->width - width1;

        node->children[0] = create_children(image, max_rmse, 0, 0, height1, width1);
        node->children[1] = create_children(image, max_rmse, 0, width1, height1, width2);
        node->children[2] = create_children(image, max_rmse, height1, 0, height2, width1);
        node->children[3] = create_children(image, max_rmse, height1, width1, height2, width2);
    }

    return node;
}

QTNode *create_children(Image *image, double max_rmse, int row, int col, int height, int width)
{
    QTNode *node = (QTNode *)malloc(sizeof(QTNode));
    node->intensity = calc_avrg_intensity(image, row, col, height, width);
    node->row = row;
    node->column = col;
    node->height = height;
    node->width = width;

    for (int i = 0; i < 4; i++)
    {
        node->children[i] = NULL;
    }

    if (height == 1 && width == 1)
    {
        return node;
    }

    if (height == 1)
    {
        if (split(image, max_rmse, row, col, height, width) == 1)
        {
            node->children[0] = create_children(image, max_rmse, row, col, height, width / 2);
            node->children[1] = create_children(image, max_rmse, row, col + width / 2, height, (width - width / 2));
            return node;
        }
    }

    else if (width == 1)
    {
        if (split(image, max_rmse, row, col, height, width) == 1)
        {
            node->children[0] = create_children(image, max_rmse, row, col, height / 2, width);
            node->children[2] = create_children(image, max_rmse, row + height / 2, col, (height - height / 2), width);
            return node;
        }
    }

    else
    {
        if (split(image, max_rmse, row, col, height, width) == 1)
        {
            node->children[0] = create_children(image, max_rmse, row, col, height / 2, width / 2);
            node->children[1] = create_children(image, max_rmse, row, col + (width / 2), height / 2, width - (width / 2));
            node->children[2] = create_children(image, max_rmse, row + (height / 2), col, height - (height / 2), width / 2);
            node->children[3] = create_children(image, max_rmse, row + (height / 2), col + (width / 2), height - (height / 2), width - (width / 2));
            return node;
        }
    }

    return node;
}

double calc_avrg_intensity(Image *image, int row, int col, int height, int width)
{
    double sum = 0;
    double count = 0;
    for (int x = row; x < row + height; x++)
    {
        for (int y = col; y < col + width; y++)
        {
            sum += image->array[x][y];
            count++;
        }
    }
    return (sum / count);
}

int split(Image *image, double max_rmse, int row, int col, int height, int width)
{
    double average = calc_avrg_intensity(image, row, col, height, width);

    double squared_difference = 0;
    double count = 0;

    for (int x = row; x < row + height; x++)
    {
        for (int y = col; y < col + width; y++)
        {
            squared_difference += (average - image->array[x][y]) * (average - image->array[x][y]);
            count++;
        }
    }

    double rmse = sqrt(squared_difference / count);

    return (rmse > max_rmse) ? 1 : 0;
}

QTNode *get_child1(QTNode *node)
{
    return node->children[0];
}

QTNode *get_child2(QTNode *node)
{
    return node->children[1];
}

QTNode *get_child3(QTNode *node)
{
    return node->children[2];
}

QTNode *get_child4(QTNode *node)
{
    return node->children[3];
}

unsigned char get_node_intensity(QTNode *node)
{
    return node->intensity;
}

void delete_quadtree(QTNode *root)
{
    if (root == NULL)
    {
        return;
    }
    delete_quadtree(root->children[0]);
    delete_quadtree(root->children[1]);
    delete_quadtree(root->children[2]);
    delete_quadtree(root->children[3]);

    free(root);
}

void save_qtree_as_ppm(QTNode *root, char *filename)
{
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "%s\n", "P3");
    fprintf(fp, "%d %d\n%hhu\n", root->width, root->height, 255);

    unsigned int height = root->height;
    unsigned int width = root->width;

    unsigned char **arr = malloc(sizeof(unsigned char *) * height);
    for (unsigned int x = 0; x < height; x++)
    {
        arr[x] = malloc(sizeof(unsigned char) * width);
    }

    for (unsigned int x = 0; x < height; x++)
    {
        for (unsigned int y = 0; y < width; y++)
        {
            arr[x][y] = recursive_find_intensity(root, x, y);
        }
    }

    for (unsigned int x = 0; x < height; x++)
    {
        for (unsigned int y = 0; y < width; y++)
        {
            fprintf(fp, "%hhu %hhu %hhu ", arr[x][y], arr[x][y], arr[x][y]);
            fprintf(fp, "\n");
        }
        
    }

    for (unsigned int i = 0; i < (unsigned int)height; i++)
    {
        free(arr[i]);
    }
    free(arr); 
}

unsigned char recursive_find_intensity(QTNode *node, int r, int c)
{
    if (node->children[0] == NULL && node->children[1] == NULL && node->children[2] == NULL && node->children[3] == NULL)
    {
        return node->intensity;
    }

    int vertical_div = node->column + (node->width - node->width / 2);  // should be the vertical division line
    int horizontal_div = node->row + (node->height - node->height / 2); // should be the horizontal division line

    // row above horiz. and left of vertical = top left node
    if (r < horizontal_div && c < vertical_div && node->children[0]) 
    {
        return recursive_find_intensity(node->children[0], r, c);
    }
    //row above horiz. and right of vertical = top right
    else if (r < horizontal_div && c >= vertical_div && node->children[1])  
    {
        return recursive_find_intensity(node->children[1], r, c);
    }
    //row below horiz. and left o fhoriz = bottom left 
    else if (r >= horizontal_div && c < vertical_div && node->children[2])
    {
        return recursive_find_intensity(node->children[2], r, c);
    }
    //below horiz but right of col so bottom rigth
    else if (r >= horizontal_div && c >= vertical_div && node->children[3])
    {
        return recursive_find_intensity(node->children[3], r, c);
    }

    return 255;
}

void save_preorder_qt(QTNode *root, char *filename)
{
    FILE *fp = fopen(filename, "w");
    save_preorder_helper(root, fp);
    fclose(fp);
}

void save_preorder_helper(QTNode *node, FILE *fp)
{
    if (node == NULL)
    {
        return;
    }
    else
    {
        if (node->children[0] == NULL && node->children[1] == NULL && node->children[2] == NULL && node->children[3] == NULL)
        {
            fprintf(fp, "L %hhu %d %d %d %d\n", node->intensity, node->row, node->height, node->column, node->width);
        }
        else
        {
            fprintf(fp, "N %hhu %d %d %d %d\n", node->intensity, node->row, node->height, node->column, node->width);
        }
        for (int i = 0; i < 4; ++i)
        {
            save_preorder_helper(node->children[i], fp);
        }
    }
}
