#include "image.h"

Image *load_image(char *filename)
{

    Image *image = malloc(sizeof(Image));
    FILE *fp = fopen(filename, "r");

    char line[3];
    fgets(line, sizeof(line), fp); // skip the whole first line, as should alwasy be P3

    unsigned int width, height, intensity;
    while (1)
    {
        if (fscanf(fp, "%u %u", &width, &height) == 2)
        { // looping through after the first line looking for width, height, skipping comments
            break;
        }

        fgets(line, sizeof(line), fp); // moves to next line

        if (line[0] == '#')
        {
            continue;
        }
    }

    fscanf(fp, "%u", &intensity);

    image->width = width;
    image->height = height;
    image->intensity = intensity;

    unsigned char **arr = malloc(sizeof(unsigned char *) * height);
    for (unsigned int x = 0; x < height; x++)
    {
        arr[x] = malloc(sizeof(unsigned char) * width);
    }

    unsigned char value;
    for (unsigned int x = 0; x < height; x++)
    {
        for (unsigned int y = 0; y < width; y++)
        {
            fscanf(fp, "%hhu", &value);
            // printf("%hhu \n", value);
            arr[x][y] = value;
            fscanf(fp, "%hhu", &value); // to skip the next 2 values since all the same
            fscanf(fp, "%hhu", &value);
        }
    }

    image->array = arr;
    fclose(fp);
    return image;
}

void delete_image(Image *image)
{
    for (unsigned int i = 0; i < (unsigned int)image->height; i++)
    {
        free(image->array[i]); // Free each row
    }
    free(image->array); // free the whole array
    free(image);
}

unsigned short get_image_width(Image *image)
{
    return image->width;
}

unsigned short get_image_height(Image *image)
{
    return image->height;
}

unsigned char get_image_intensity(Image *image, unsigned int row, unsigned int col)
{
    return image->array[row][col];
}

unsigned int hide_message(char *message, char *input_filename, char *output_filename)
{
    // code just to isolate headers
    FILE *fp = fopen(input_filename, "r");
    FILE *output = fopen(output_filename, "w");

    char line[3];
    fgets(line, sizeof(line), fp);

    unsigned int width, height, intensity;
    while (1)
    {
        if (fscanf(fp, "%u %u", &width, &height) == 2)
        {
            break;
        }

        fgets(line, sizeof(line), fp); // moves to next line

        if (line[0] == '#')
        {
            continue;
        }
    }

    fscanf(fp, "%u", &intensity);

    fprintf(output, "%s\n", "P3");
    fprintf(output, "%d %d\n", width, height);
    fprintf(output, "%d\n", intensity);

    Image *imageStruct = load_image(input_filename);
    unsigned char **image = imageStruct->array;

    unsigned int total = (height * width);
    signed int temp = (signed)total / 8;
    unsigned int max_chars;
    if (temp < 0)
    {
        max_chars = 0;
    }
    else
    {
        max_chars = (unsigned)temp;
    }
    unsigned int char_at = 0;      // holds current position of the char in message that is being encoded.
    unsigned int pos_in_ASCII = 0; // holds the bit index(left to right) of the current position in the current char
    int added_null = 0;            // flag if we finished encoding null terminator or not
    unsigned char value;           // to hold current value
    int char_done = 0;             // flag if we finished with ALL chars/reached max amount of chars possibl to be encoded
    int added_null_count = 0;      // count of null bits encoded
    int first_time_null = 0;       // flag if first time encoding null
    int first_time_og = 0;         // flag if first time encoding og values

    for (unsigned int x = 0; x < height; x++)
    {
        for (unsigned int y = 0; y < width; y++)
        {
            if (char_done == 0)
            {

                if (char_at >= strlen(message) || (signed)char_at >= (signed)max_chars - 1)
                {
                    char_done = 1;
                }

                if (char_done == 0 && pos_in_ASCII < 8)
                {
                    unsigned char new_value = (image[x][y] & 0xFE) | ((message[char_at] >> (7 - pos_in_ASCII)) & 1);
                    printf("Changing image[%d][%d] which is %d to %hhu by encoding the LSB of the og. value with the bit at pos %d(left to right) of char '%c'. \n", x, y, image[x][y], new_value, pos_in_ASCII, message[char_at]);
                    fprintf(output, "%hhu ", new_value);
                    fprintf(output, "%hhu ", new_value);
                    fprintf(output, "%hhu ", new_value);
                    pos_in_ASCII++;

                    if (pos_in_ASCII == 8)
                    {
                        pos_in_ASCII = 0;
                        char_at++;
                    }
                }
            }

            else if (added_null == 0)
            {

                if (first_time_null == 0)
                {
                    first_time_null = 1;
                    if (y > 0)
                    {
                        y--;
                    }
                    else if (x > 0)
                    {
                        x--;
                        y = width - 1;
                    }
                }

                if (added_null_count == 8)
                {
                    added_null = 1;
                }
                else
                {
                    unsigned char new_value = (image[x][y] & 0xFE);
                    printf("Encoding null terminator: Changing image[%d][%d]: %d to %hhu \n", x, y, image[x][y], new_value);
                    fprintf(output, "%hhu ", new_value);
                    fprintf(output, "%hhu ", new_value);
                    fprintf(output, "%hhu ", new_value);

                    added_null_count++;
                }
            }
            else
            {
                if (first_time_og == 0)
                {
                    first_time_og = 1;
                    if (y > 0)
                    {
                        y--;
                    }
                    else if (x > 0)
                    {
                        x--;
                        y = width - 1;
                    }
                }

                value = image[x][y];
                printf("Writing oriigjnal [%d][%d]: %d \n", x, y, image[x][y]);
                fprintf(output, "%hhu ", value);
                fprintf(output, "%hhu ", value);
                fprintf(output, "%hhu ", value);
            }
        }
        fprintf(output, "\n");
    }
    delete_image(imageStruct);
    fclose(fp);
    fclose(output);
    printf("MAXXHARS %u", max_chars - 1);
    printf("\n %u", char_at);
    return char_at;
}

char *reveal_message(char *input_filename)
{
    FILE *fp = fopen(input_filename, "r");

    char line[3];
    fgets(line, sizeof(line), fp);

    unsigned int width, height, intensity;
    while (1)
    {
        if (fscanf(fp, "%u %u", &width, &height) == 2)
        {
            break;
        }

        fgets(line, sizeof(line), fp);

        if (line[0] == '#')
        {
            continue;
        }
    }
    fscanf(fp, "%u", &intensity);

    unsigned int total = (height * width);
    unsigned int max_chars = total / 8;

    Image *imageStruct = load_image(input_filename);
    unsigned char **image = imageStruct->array;

    char *message = malloc(sizeof(char) * (max_chars));

    unsigned int char_at = 0;      // holds current position of the char in message that is being encoded.
    unsigned int pos_in_ASCII = 0; // holds the bit index(left to right) of the current position in the current char
    unsigned char curr_char;

    for (unsigned int x = 0; x < height && char_at < max_chars; x++)
    {
        for (unsigned int y = 0; y < width && char_at < max_chars; y++)
        {
            if (pos_in_ASCII == 8)
            {
                pos_in_ASCII = 0;
                message[char_at++] = curr_char;
                curr_char = 0;
            }

            int lastBit = image[x][y] & 0x01;
            curr_char |= (lastBit << (7 - pos_in_ASCII));
            pos_in_ASCII++;
        }
    }

    delete_image(imageStruct);
    fclose(fp);
    message[max_chars - 1] = '\0';
    return message;
}

unsigned int hide_image(char *secret_image_filename, char *input_filename, char *output_filename)
{
    Image *secret = load_image(secret_image_filename);
    Image *input = load_image(input_filename);
    FILE *sec = fopen(secret_image_filename, "r");
    FILE *out = fopen(output_filename, "w");

    fprintf(out, "%s\n", "P3");
    fprintf(out, "%d %d\n", input->width, input->height);
    fprintf(out, "%hhu\n", input->intensity);

    // skip header to read over the secret file later on
    char line[3];
    fgets(line, sizeof(line), sec);
    unsigned int width, height, intensity;
    while (1)
    {
        if (fscanf(sec, "%u %u", &width, &height) == 2)
        {
            break;
        }

        fgets(line, sizeof(line), sec); // moves to next line

        if (line[0] == '#')
        {
            continue;
        }
    }
    fscanf(sec, "%u", &intensity);

    unsigned int pixel_at = 0;                              // holds current position of the pixel in the secret image that is being encoded.
    int pixels_done = 0;                                    // flag if we finished with pixels
    int pixel_count = (8 * secret->width * secret->height); // how many pixels it will take to encode the secret image
    int width_done = 0;                                     // flag if secret's width done being encoded
    int height_done = 0;                                    // flag if secret's height done being encoded

    // flag for first time transitioning to writting original values
    int first_og = 0;

    int pos_in_pixel = 0;
    unsigned char initial_val;
    fscanf(sec, "%hhu", &initial_val);
    fscanf(sec, "%hhu", &initial_val);
    fscanf(sec, "%hhu", &initial_val);

    if ((unsigned)pixel_count + 16 > input->width * input->height)
    {
        delete_image(input);
        delete_image(secret);
        fclose(sec);
        fclose(out);
        return 0;
    }

    for (unsigned int x = 0; x < (unsigned)input->height; x++)
    {
        for (unsigned int y = 0; y < input->width; y++)
        {
            if (width_done == 0)
            {

                unsigned char new_value = (input->array[x][y] & 0xFE) | ((secret->width >> (7 - pos_in_pixel)) & 1);
                // printf("Encoding WIDTH input[%d][%d]: %hhu to %hhu (encoded bit pos %d of %hhu)\n", x,y,input->array[x][y], new_value, pos_in_pixel, secret->width);
                fprintf(out, "%hhu ", new_value);
                fprintf(out, "%hhu ", new_value);
                fprintf(out, "%hhu ", new_value);
                pos_in_pixel++;

                if (pos_in_pixel == 8)
                {
                    pos_in_pixel = 0;
                    width_done = 1;
                }
            }

            else if (height_done == 0)
            {

                unsigned char new_value = (input->array[x][y] & 0xFE) | ((secret->height >> (7 - pos_in_pixel)) & 1);
                // printf("Encoding HEIGHT input[%d][%d]: %hhu to %hhu (encoded bit pos %d of %hhu)\n", x,y,input->array[x][y], new_value, pos_in_pixel, secret->height);
                fprintf(out, "%hhu ", new_value);
                fprintf(out, "%hhu ", new_value);
                fprintf(out, "%hhu ", new_value);
                pos_in_pixel++;

                if (pos_in_pixel == 8)
                {
                    pos_in_pixel = 0;
                    height_done = 1;
                }
            }

            else if (pixels_done == 0)
            {

                if (pixel_at * 8 >= (unsigned)pixel_count)
                {
                    pixels_done = 1;
                }

                if (pixels_done == 0 && pos_in_pixel < 8)
                {
                    unsigned char new_value = (input->array[x][y] & 0xFE) | ((initial_val >> (7 - pos_in_pixel)) & 1);
                    // printf("Encoding input[%d][%d]: %hhu to %hhu (encoded bit pos %d of %hhu)\n", x,y,input->array[x][y], new_value, pos_in_pixel, initial_val);
                    fprintf(out, "%hhu ", new_value);
                    fprintf(out, "%hhu ", new_value);
                    fprintf(out, "%hhu ", new_value);

                    pos_in_pixel++;
                    if (pos_in_pixel == 8)
                    {
                        pos_in_pixel = 0;
                        pixel_at++;

                        if (pixel_at < secret->width * secret->height)
                        {
                            fscanf(sec, "%hhu", &initial_val);
                            fscanf(sec, "%hhu", &initial_val);
                            fscanf(sec, "%hhu", &initial_val);
                        }
                    }
                }
            }
            else
            {
                if (first_og == 0)
                {
                    first_og = 1;
                    if (y > 0)
                    {
                        y--;
                    }
                    else if (x > 0)
                    {
                        x--;
                        y = width - 1;
                    }
                }
                fprintf(out, "%hhu ", input->array[x][y]);
                fprintf(out, "%hhu ", input->array[x][y]);
                fprintf(out, "%hhu ", input->array[x][y]);
            }
        }
        fprintf(out, "\n");
    }
    delete_image(input);
    delete_image(secret);
    fclose(sec);
    fclose(out);
    return 1;
}

void reveal_image(char *input_filename, char *output_filename)
{
    Image *image = load_image(input_filename);
    FILE *output = fopen(output_filename, "w");

    fprintf(output, "%s\n", "P3");

    int pixels_done = 0;             // flag if we finished with pixels
    unsigned char curr_pixel = 0;    // current pixel
    int pos_in_pixel = 0;            // bit pos in curr_pixel
    unsigned int pixels_decoded = 0; // num of pixels decoded
    int width_done = 0;              // flag if secret's width done being encoded
    int height_done = 0;             // flag if secret's height done being encoded

    // flag for transitioning to height process
    int first_height = 0;
    int first_pixel = 0;

    unsigned int width = 0;
    unsigned int height = 0;

    int stop_loop = 0;

    for (unsigned int x = 0; x < image->height && stop_loop == 0; x++)
    {
        for (unsigned int y = 0; y < image->width && stop_loop == 0; y++)
        {
            if (width_done == 0)
            {
                if (pos_in_pixel == 8)
                {
                    pos_in_pixel = 0;
                    fprintf(output, "%d ", width);
                    width_done = 1;
                }
                if (width_done == 0)
                {
                    int lastBit = image->array[x][y] & 0x01;
                    width |= (lastBit << (7 - pos_in_pixel));
                    pos_in_pixel++;
                }
            }
            else if (height_done == 0)
            {
                if (first_height == 0)
                {
                    first_height = 1;
                    if (y > 0)
                    {
                        y--;
                    }
                    else if (x > 0)
                    {
                        x--;
                        y = width - 1;
                    }
                }

                if (pos_in_pixel == 8)
                {
                    pos_in_pixel = 0;
                    fprintf(output, "%d\n", height);
                    fprintf(output, "%hhu\n", image->intensity);
                    height_done = 1;
                }
                if (height_done == 0)
                {
                    int lastBit = image->array[x][y] & 0x01;
                    height |= (lastBit << (7 - pos_in_pixel));
                    pos_in_pixel++;
                }
            }
            else if (pixels_done == 0)
            {

                if (first_pixel == 0)
                {
                    first_pixel = 1;
                    if (y > 0)
                    {
                        y--;
                    }
                    else if (x > 0)
                    {
                        x--;
                        y = width - 1;
                    }
                }

                int total_pixels = height * width; // total pixels of hidden image

                if (pixels_decoded >= (unsigned)total_pixels)
                {
                    pixels_done = 1;
                }

                if (pos_in_pixel == 8 && pixels_done == 0)
                {
                    fprintf(output, "%hhu %hhu %hhu ", curr_pixel, curr_pixel, curr_pixel);
                    curr_pixel = 0;
                    pos_in_pixel = 0;
                    pixels_decoded++;
                }

                if (pixels_done == 0)
                {
                    int lastBit = image->array[x][y] & 0x01;
                    curr_pixel |= (lastBit << (7 - pos_in_pixel));
                    pos_in_pixel++;
                }
            }
            else
            {
                stop_loop = 1;
                break;
            }
        }
        fprintf(output, "\n");
    }
    fclose(output);
    delete_image(image);
}