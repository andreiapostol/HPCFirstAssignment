
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Define output file name
#define OUTPUT_FILE "stencil.pgm"

void stencil(const int nx, const int ny, float image[][nx], float tmp_image[][nx]);
void init_image(const int nx, const int ny, float image[][nx], float tmp_image[][nx]);
void output_image(const char * file_name, const int nx, const int ny, float image[][nx]);
double wtime(void);

int main(int argc, char *argv[]) {

  printf("enters main");

  // Check usage
  if (argc != 4) {
    fprintf(stderr, "Usage: %s nx ny niters\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Initiliase problem dimensions from command line arguments
  int nx = atoi(argv[1]);
  int ny = atoi(argv[2]);
  int niters = atoi(argv[3]);

  // Allocate the image
  // float *image = malloc(sizeof(float)*(nx+2)*(ny+2));
  // float *tmp_image = malloc(sizeof(float)*(nx+2)*(ny+2));

  float image[nx+2][ny+2];
  float tmp_image[nx+2][ny+2];

  // Set the input image
  init_image(nx+2, ny+2, image, tmp_image);

  // Call the stencil kernel
  printf("%d", niters);
  double tic = wtime();
  for (int t = 0; t < niters; ++t) {
    printf("%d iteration\n", t);
    stencil(nx+2, ny+2, image, tmp_image);
    stencil(nx+2, ny+2, tmp_image, image);
  }
  // for(int t = 0; t < 2 * niters - 1; ++t){
  //   newStencil(nx+2, ny+2, image, currentRow);
  // }

  double toc = wtime();



  // Output
  printf("------------------------------------\n");
  printf(" runtime: %lf s\n", toc-tic);
  printf("------------------------------------\n");

  output_image(OUTPUT_FILE, nx+2, ny+2, image);
}

void stencil(const int nx, const int ny, float image[][nx], float tmp_image[][nx]) {
  // UPDATE 2 : change order of fors
  // printf("Entered stencil");
  for (int i = 1; i < nx-1; i++) {
    for (int j = 1; j < ny-1; j++) {
      // UPDATE 1 : 3.0 / 5 -> 0.6
      // printf("%d %d  ", i, j);
      tmp_image[i][j] = image[i][j] * 0.6 +
        (image[i-1][j] +
         image[i+1][j] +
         image[i][j-1] +
         image[i][j+1]) * 0.1;
    }
  }
}

// Create the input image
void init_image(const int nx, const int ny, float image[][nx], float tmp_image[][nx]) {
  // Zero everything
  for (int j = 0; j < ny; ++j) {
    for (int i = 0; i < nx; ++i) {
      image[i][j] = 0.0;
      tmp_image[i][j] = 0.0;
    }
  }

  // Checkerboard
  for (int j = 0; j < 8; ++j) {
    for (int i = 0; i < 8; ++i) {
      for (int jj = j*(ny-2)/8; jj < (j+1)*(ny-2)/8; ++jj) {
        for (int ii = i*(nx-2)/8; ii < (i+1)*(nx-2)/8; ++ii) {
          if ((i+j)%2)
          image[ii+1][jj+1] = 100.0;
        }
      }
    }
  }
}

// Routine to output the image in Netpbm grayscale binary image format
void output_image(const char * file_name, const int nx, const int ny, float image[][nx]) {

  // Open output file
  FILE *fp = fopen(file_name, "w");
  if (!fp) {
    fprintf(stderr, "Error: Could not open %s\n", OUTPUT_FILE);
    exit(EXIT_FAILURE);
  }

  // Ouptut image header
  fprintf(fp, "P5 %d %d 255\n", nx-2, ny-2);

  // Calculate maximum value of image
  // This is used to rescale the values
  // to a range of 0-255 for output
  double maximum = 0.0;
  for (int j = 1; j < ny-1; ++j) {
    for (int i = 1; i < nx-1; ++i) {
      if (image[j][i] > maximum)
        maximum = image[i][j];
    }
  }
  // Output image, converting to numbers 0-255
  for (int j = 1; j < ny-1; ++j) {
    for (int i = 1; i < nx-1; ++i) {
      fputc((char)(255.0*image[i][j]/maximum), fp);
    }
  }

  // Close the file
  fclose(fp);

}

// Get the current time in seconds since the Epoch
double wtime(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec*1e-6;
}
