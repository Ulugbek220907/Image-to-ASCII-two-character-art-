#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_WIDTH 1920
#define MAX_HEIGHT 1080
#define MAX_FILENAME 256

typedef struct {
    unsigned char r, g, b;
} Pixel;

typedef struct {
    Pixel *data;
    int width;
    int height;
} Image;

int get_brightness(Pixel p) {
    return (int)(0.299 * p.r + 0.587 * p.g + 0.114 * p.b);
}

bool is_ppm_or_pgm(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (!ext) return false;
    return (strcasecmp(ext, ".ppm") == 0 || strcasecmp(ext, ".pgm") == 0);
}

Image *load_ppm_pgm(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return NULL;
    }

    Image *img = (Image *)malloc(sizeof(Image));
    if (!img) {
        fclose(file);
        return NULL;
    }

    char magic[3], comment[256];
    int max_val;

    if (fscanf(file, "%2s", magic) != 1) {
        printf("Error: Invalid PPM/PGM format\n");
        free(img);
        fclose(file);
        return NULL;
    }

    while (fgetc(file) != '\n');

    while (fgetc(file) == '#') {
        fgets(comment, sizeof(comment), file);
    }
    fseek(file, -1, SEEK_CUR);

    if (fscanf(file, "%d %d %d", &img->width, &img->height, &max_val) != 3) {
        printf("Error: Could not read image dimensions\n");
        free(img);
        fclose(file);
        return NULL;
    }
    fgetc(file);

    if (img->width <= 0 || img->height <= 0 || img->width > MAX_WIDTH || img->height > MAX_HEIGHT) {
        printf("Error: Invalid or too large image dimensions\n");
        free(img);
        fclose(file);
        return NULL;
    }

    img->data = (Pixel *)malloc(img->width * img->height * sizeof(Pixel));
    if (!img->data) {
        free(img);
        fclose(file);
        return NULL;
    }

    bool is_pgm = (magic[1] == '5');
    bool is_ppm = (magic[1] == '6');

    if (is_pgm) {
        for (int i = 0; i < img->width * img->height; i++) {
            unsigned char gray = fgetc(file);
            img->data[i].r = img->data[i].g = img->data[i].b = gray;
        }
    } else if (is_ppm) {
        if (fread(img->data, sizeof(Pixel), img->width * img->height, file) 
            != (size_t)(img->width * img->height)) {
            printf("Error: Could not read image data\n");
            free(img->data);
            free(img);
            fclose(file);
            return NULL;
        }
    } else {
        printf("Error: Unsupported PPM/PGM format\n");
        free(img->data);
        free(img);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return img;
}

Image *load_image(const char *filename) {
    if (is_ppm_or_pgm(filename)) {
        return load_ppm_pgm(filename);
    }

    printf("Supported formats: .ppm, .pgm\n");
    printf("Note: PNG/JPG support requires external libraries (libjpeg, libpng)\n");
    return NULL;
}

void convert_to_ascii(Image *img, char char1, char char2, int scale, FILE *output) {
    if (!img || !output) return;

    int threshold = 128;

    for (int y = 0; y < img->height; y += scale) {
        for (int x = 0; x < img->width; x += scale) {
            int brightness = get_brightness(img->data[y * img->width + x]);
            char c = (brightness > threshold) ? char1 : char2;
            fprintf(output, "%c", c);
        }
        fprintf(output, "\n");
    }
}

void free_image(Image *img) {
    if (img) {
        free(img->data);
        free(img);
    }
}

int main() {
    char filename[MAX_FILENAME];
    char char1, char2;
    int scale;
    char output_filename[MAX_FILENAME];
    
    printf("========================================\n");
    printf("  Image to ASCII Art Converter v1.0\n");
    printf("========================================\n\n");

    printf("Enter image filename (PPM/PGM): ");
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        printf("Error reading filename\n");
        return 1;
    }
    filename[strcspn(filename, "\n")] = '\0';

    if (strlen(filename) == 0) {
        printf("Error: Filename cannot be empty\n");
        return 1;
    }

    Image *img = load_image(filename);
    if (!img) {
        printf("Error: Could not load image\n");
        return 1;
    }

    printf("Image loaded: %dx%d pixels\n\n", img->width, img->height);

    printf("Enter first character (for bright pixels): ");
    if (scanf("%c", &char1) != 1) {
        printf("Error reading character\n");
        free_image(img);
        return 1;
    }
    getchar();

    printf("Enter second character (for dark pixels): ");
    if (scanf("%c", &char2) != 1) {
        printf("Error reading character\n");
        free_image(img);
        return 1;
    }
    getchar();

    printf("Enter sampling scale (1-10, higher = smaller output): ");
    if (scanf("%d", &scale) != 1 || scale < 1 || scale > 10) {
        printf("Invalid scale. Using default scale of 2\n");
        scale = 2;
    }
    getchar();

    printf("Enter output filename (default: output.txt): ");
    if (fgets(output_filename, sizeof(output_filename), stdin) == NULL) {
        strcpy(output_filename, "output.txt");
    } else {
        output_filename[strcspn(output_filename, "\n")] = '\0';
        if (strlen(output_filename) == 0) {
            strcpy(output_filename, "output.txt");
        }
    }

    FILE *output_file = fopen(output_filename, "w");
    if (!output_file) {
        printf("Error: Could not create output file %s\n", output_filename);
        free_image(img);
        return 1;
    }

    printf("\nConverting image to ASCII art...\n");
    convert_to_ascii(img, char1, char2, scale, output_file);
    fclose(output_file);

    printf("✓ Conversion complete!\n");
    printf("✓ Output saved to: %s\n", output_filename);
    printf("✓ ASCII dimensions: %dx%d characters\n", 
           img->width / scale, img->height / scale);

    free_image(img);
    return 0;
}
