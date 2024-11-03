#include "qtree.h"
#include "image.h"

#include "tests_utils.h"

int main() {
    struct stat st;
    if (stat("tests/output", &st) == -1)
        mkdir("tests/output", 0700);
    prepare_input_image_file("building1.ppm"); // copies the image to the images/ directory

    /******************************* create_quadtree *******************************/
    double max_rmse = 25;
    Image *image = load_image("images/building1.ppm");
    QTNode *root = create_quadtree(image, max_rmse);
    delete_quadtree(root);
    delete_image(image);

    /******************************* save_preorder_qt *******************************/
    image = load_image("images/building1.ppm"); 
    root = create_quadtree(image, 25);
    save_preorder_qt(root, "tests/output/save_preorder_qt1_qtree.txt");
    delete_quadtree(root);
    delete_image(image);

    /******************************* save_qtree_as_ppm *******************************/
    image = load_image("images/building1.ppm"); 
    root = create_quadtree(image, 25);
    save_qtree_as_ppm(root, "tests/output/save_qtree_as_ppm1.ppm");
    delete_image(image); 
    delete_quadtree(root);

    /******************************* hide_message and reveal_message *******************************/
    prepare_input_image_file("wolfie-tiny.ppm");
    hide_message("0000000000111111111122222222223333333333", "images/wolfie-tiny.ppm", "tests/output/hide_message1.ppm");
    char *message = reveal_message("tests/output/hide_message1.ppm");
    printf("Message: %s\n", message);
    free(message);

    /******************************* hide_image and reveal_image *******************************/
    hide_image("images/wolfie-tiny.ppm", "images/building1.ppm", "tests/output/hide_image1.ppm");
    reveal_image("tests/output/hide_image1.ppm", "tests/output/reveal_image1.ppm");

    return 0;
}