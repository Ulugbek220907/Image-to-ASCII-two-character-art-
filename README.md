# Quick Reference Guide: Image to ASCII Converter

## 🚀 Quick Start

### 1. Compile
```bash
gcc -o main main.c -Wall -Wextra -O2
```

### 2. Prepare Image
```bash
convert photo.jpg image.ppm
```

### 3. Run
```bash
./main
```

### 4. Follow Prompts
```
Filename: image.ppm
Bright char: #
Dark char: .
Scale: 2
Output: output.txt
```

---

## 📋 Command Reference

| Task | Command |
|------|---------|
| **Compile** | `gcc -o main main.c -Wall -Wextra -O2` |
| **Run** | `./main` |
| **View output** | `cat output.txt` |
| **Convert JPG to PPM** | `convert photo.jpg image.ppm` |
| **Resize image** | `convert -resize 800x600 large.ppm small.ppm` |
| **Create PPM from scratch** | `gimp` → Export as PPM |
| **Check file format** | `file image.ppm` |

---

## 🔑 Key Concepts Cheat Sheet

### Pointers

```c
// Declaration
int *ptr;              // Pointer to int
char *str;             // Pointer to char (string)
Image *img;            // Pointer to Image struct

// Address-of operator (&)
int x = 5;
int *ptr = &x;         // ptr holds address of x

// Dereference operator (*)
int value = *ptr;      // value = 5

// Arrow operator (->)
img->width = 800;      // Access through pointer
(*img).width = 800;    // Equivalent
```

### Arrays vs Pointers

```c
// Array (fixed size, stack)
char filename[256];    // 256 bytes on stack
filename[0] = 'a';

// Pointer (dynamic, heap)
char *filename = malloc(256);  // Allocate on heap
filename[0] = 'a';
free(filename);        // Must free!
```

### Memory Allocation

```c
// Single value
Image *img = malloc(sizeof(Image));

// Array of values
Pixel *pixels = malloc(width * height * sizeof(Pixel));

// Always check for NULL
if (!img) {
    printf("Error: Out of memory\n");
    return 1;
}

// Always free when done
free(img);
```

### Structs

```c
// Define struct
typedef struct {
    unsigned char r, g, b;
} Pixel;

// Create variable
Pixel p;
p.r = 255;
p.g = 128;
p.b = 0;

// Access through pointer
Pixel *ptr = &p;
ptr->r = 255;      // Using arrow operator
(*ptr).r = 255;    // Equivalent
```

---

## 🎨 Character Selection Guide

### High Contrast
- Bright: `#`
- Dark: `.`
- **Effect**: Bold, clear output
- **Best for**: Simple images, maximum visibility

### Subtle
- Bright: `*`
- Dark: ` ` (space)
- **Effect**: Artistic, less aggressive
- **Best for**: Photographs, detail preservation

### Very Bold
- Bright: `@`
- Dark: ` ` (space)
- **Effect**: Maximum contrast
- **Best for**: Low-resolution, dark images

### Detailed
- Bright: `&`
- Dark: `.`
- **Effect**: Medium contrast, good detail
- **Best for**: Medium complexity images

---

## 📊 Scale Selection Guide

```
Scale 1:  Highest detail, largest output (original resolution)
Scale 2:  Balanced (default, recommended)
Scale 5:  Low detail, fast processing
Scale 10: Minimal detail, ultra-compact
```

**Formula**: Output size = (Width / Scale) × (Height / Scale)

**Examples**:
```
800×600 image:
  Scale 1  → 800×600 (huge)
  Scale 2  → 400×300 (normal)
  Scale 5  → 160×120 (compact)
  Scale 10 → 80×60   (tiny)
```

---

## 🔍 Understanding the Code Flow

### Main Loop Structure

```c
for (int y = 0; y < img->height; y += scale) {
    for (int x = 0; x < img->width; x += scale) {
        // Process pixel at (x, y)
        int brightness = get_brightness(img->data[y * img->width + x]);
        
        // Choose character
        char c = (brightness > 128) ? char1 : char2;
        
        // Write to file
        fprintf(output, "%c", c);
    }
    fprintf(output, "\n");  // End of row
}
```

### Pixel Array Access

```c
// 1D array, 2D interpretation
// Image: width × height pixels
// Array: single flat array

// To access pixel at column x, row y:
int index = y * width + x;
Pixel p = img->data[index];
```

**Visual**:
```
3×3 image:
[0][1][2]
[3][4][5]  ← row 1
[6][7][8]

Pixel at (x=1, y=1) = data[1*3 + 1] = data[4]
```

---

## 📁 File Format Quick Reference

### PPM (Color) - Magic P6

```
P6                 ← Magic number (always P6)
800 600            ← Width Height
255                ← Max value (always 255)
[binary RGB data]  ← 800*600*3 = 1,440,000 bytes
```

### PGM (Grayscale) - Magic P5

```
P5                 ← Magic number (always P5)
800 600            ← Width Height
255                ← Max value (always 255)
[binary gray data] ← 800*600 = 480,000 bytes
```

### Header Parsing

```c
// Read magic
fscanf(file, "%2s", magic);     // Read "P6" or "P5"

// Skip comments
while (fgetc(file) == '#') {
    fgets(comment, sizeof(comment), file);
}

// Read dimensions
fscanf(file, "%d %d %d", &width, &height, &max_val);

// Skip newline
fgetc(file);

// Read pixel data
fread(pixels, sizeof(Pixel), width * height, file);
```

---

## 🐛 Debugging Tips

### Check if Image Loaded

```c
Image *img = load_image(filename);
if (!img) {
    printf("Failed to load image\n");
    return 1;
}
printf("Loaded: %d × %d\n", img->width, img->height);
```

### Print Pixel Info

```c
Pixel p = img->data[0];
printf("First pixel RGB: (%d, %d, %d)\n", p.r, p.g, p.b);

int brightness = get_brightness(p);
printf("Brightness: %d\n", brightness);
```

### Trace Conversion

```c
// Add inside convert_to_ascii()
if (x < 10 && y < 10) {  // Only first few
    printf("Pixel(%d,%d) = brightness %d → '%c'\n", 
           x, y, brightness, c);
}
```

### Check Memory

```c
// Before malloc
printf("Allocating %ld bytes\n", width * height * sizeof(Pixel));

// After malloc
if (!img->data) {
    printf("malloc failed!\n");
    return NULL;
}
```

---

## 💾 Memory Cheat Sheet

### Size Calculations

```c
sizeof(char)   = 1 byte
sizeof(int)    = 4 bytes
sizeof(Pixel)  = 3 bytes (3 × char)
sizeof(Image)  = ~20 bytes (pointer + 2 ints)

// For 800×600 image:
Pixel array = 800 * 600 * 3 = 1,440,000 bytes ≈ 1.4 MB
```

### Allocation Pattern

```c
// Step 1: Allocate struct
Image *img = malloc(sizeof(Image));
if (!img) return NULL;

// Step 2: Allocate data pointer
img->data = malloc(width * height * sizeof(Pixel));
if (!img->data) {
    free(img);
    return NULL;
}

// Step 3: Use

// Step 4: Free in reverse order
free(img->data);
free(img);
```


---

## 🔗 Quick Links

- **Main Code**: `main.c`
- **Full Documentation**: `README.md`

---

## 📞 Common Questions

### Q: Why PPM and not PNG/JPG?
**A**: PPM is simple (raw pixels, no compression), making it ideal for learning. PNG/JPG require external libraries (libpng, libjpeg).

### Q: What if I don't have ImageMagick?
**A**: Use any image editor:
- GIMP: File → Export As → PPM
- Online: Convert online at cloudconvert.com

### Q: Can I use more than 2 characters?
**A**: Current version: No. Future version will support gradient of characters.

### Q: How do I make color ASCII art?
**A**: Current version: Convert RGB to grayscale only. Future version will use ANSI color codes.

### Q: What's the maximum image size?
**A**: 1920×1080 by default (change MAX_WIDTH/MAX_HEIGHT constants).

### Q: How do I run without prompts?
**A**: Not yet. Future version will have command-line argument support.


---

**Good luck and happy converting!** 🚀
