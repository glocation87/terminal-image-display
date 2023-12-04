#include <assert.h>
#include <stdio.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "./resources/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "./resources/stb_image_resize.h"
#include "./resources/maps.h"

#define DEFAULT_WIDTH 32

typedef enum {
    DIST_HSL,
    DIST_RGB
} EnumDistance;

static int distance256(int table256[256][3], int i, int a, int b, int c)
{
    int da = a - table256[i][0];
    int db = b - table256[i][1];
    int dc = c - table256[i][2];
    return da*da + db*db + dc*dc;
}

static int find_ansi_index(int table256[256][3], int a, int b, int c)
{
    int index = 0;
    for (int i = 0; i < 256; ++i) {
        if (distance256(table256, i, a, b, c) < distance256(table256, index, a, b, c)) {
            index = i;
        }
    }
    return index;
}


const char* ParseArgs(int* argc, char** argv[])
{
    assert(*argc > 0);
    const char* result = **argv;
    *argc -= 1;
    *argv += 1;
    return result;
}

int main(int argc, char* argv[])
{
    const char* identifier = ParseArgs(&argc, &argv);
    EnumDistance distEnum = DIST_RGB;

    //step through the passed args
    while (argc > 0) {

        const char* flag = ParseArgs(&argc, &argv);
        if (strcmp(flag, "-hsl") ==  0) {
            distEnum = DIST_HSL;
        } else {
            const char* pathToFile = flag;
            int width, height;
            uint32_t* pixels = (uint32_t*)stbi_load(pathToFile, &width, &height, NULL, 4);

            if (pixels == NULL) {
                fprintf(stderr, "Could not read file: %s\n", pathToFile);
                continue;
            }
            //Scale the image to fit into our console
            int resized_height = height*DEFAULT_WIDTH/width;
            uint32_t* resized_pixels = malloc(sizeof(uint32_t)*DEFAULT_WIDTH*resized_height); //alloc memory for our pixels
            if (resized_pixels == NULL) {
                fprintf(stderr, "Could not allocate memory for resized image\n");
                exit(1);
            }

            int result = stbir_resize_uint8(
                (const unsigned char*)pixels, width, height, sizeof(uint32_t)*width,
                (unsigned char*)resized_pixels, DEFAULT_WIDTH, resized_height, sizeof(uint32_t)*DEFAULT_WIDTH,
                4);

            printf("%d", result);
            
            for (int y = 0; y < resized_height; ++y) {
                for (int x = 0; x < DEFAULT_WIDTH; ++x) {
                    uint32_t pixel = resized_pixels[y*DEFAULT_WIDTH + x];
                    int r = (pixel>>8*0)&0xFF;
                    int g = (pixel>>8*1)&0xFF;
                    int b = (pixel>>8*2)&0xFF;
                    int a = (pixel>>8*3)&0xFF;
                    //convert
                    r = a*r/255; 
                    g = a*g/255;
                    b = a*b/255;
                    //To-Do: Add support for HSL
                    printf("\e[48;5;%dm  ", find_ansi_index(rgb256, r, g, b));
                }
                printf("\e[0m\n");
            }
            printf("\e[0m\n");
            free(resized_pixels);
            stbi_image_free(pixels);
        }
    }
    return 0;
}