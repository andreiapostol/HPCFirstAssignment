
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Define output file name
#define OUTPUT_FILE "stencil.pgm"

void newStencil(const int nx, const int ny, double* image, double* restrict currentRow);
void stencil(const int nx, const int ny, double *  image, double * restrict  tmp_image);
void init_image(const int nx, const int ny, double * image);
void output_image(const char * file_name, const int nx, const int ny, double *image);
double wtime(void);

int main(int argc, char *argv[]) {

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
  double *image = malloc(sizeof(double)*(nx+2)*(ny+2));
  double *currentRow = malloc(sizeof(double)*(ny+2));

  for(int i = 0; i < ny+2; i++){
    currentRow[i] = 0;
  }

  // Set the input image
  init_image(nx+2, ny+2, image);

  // Call the stencil kernel
  double tic = wtime();
  // for (int t = 0; t < niters; ++t) {
  //   stencil(nx, ny, image, tmp_image);
  //   stencil(nx, ny, tmp_image, image);
  // }
  for(int t = 0; t < 2 * niters - 1; ++t){
    newStencil(nx+2, ny+2, image, currentRow);
  }

  double toc = wtime();



  // Output
  printf("------------------------------------\n");
  printf(" runtime: %lf s\n", toc-tic);
  printf("------------------------------------\n");

  output_image(OUTPUT_FILE, nx+2, ny+2, image);
  free(image);
}

// void newStencil(const int nx, const int ny, double* restrict image, double* restrict currentRow){
//   double toSetTop;
//   for (int j = 0; j < ny; j++) {
//     int i = 0;
//     toSetTop = image[j+i*ny];
//     // UPDATE 1 : 3.0 / 5 -> 0.6
//     image[j+i*ny] = image[j+i*ny] * 0.6;
//     if (i < nx-1) image[j+i*ny] += image[j  +(i+1)*ny] * 0.1;
//     if (j > 0)    image[j+i*ny] += currentRow[j - 1] * 0.1;
//     if (j < ny-1) image[j+i*ny] += image[j+1+i*ny] * 0.1;
//     currentRow[j] = toSetTop;
//   }
//   for (int i = 1; i < nx-1; i++) {
//     for (int j = 0; j < ny; j++) {
//       toSetTop = image[j+i*ny];
//       // UPDATE 1 : 3.0 / 5 -> 0.6
//       image[j+i*ny] = image[j+i*ny] * 0.6;
//       image[j+i*ny] += currentRow[j] * 0.1;
//       image[j+i*ny] += image[j  +(i+1)*ny] * 0.1;
//       if (j > 0)    image[j+i*ny] += currentRow[j - 1] * 0.1;
//       if (j < ny-1) image[j+i*ny] += image[j+1+i*ny] * 0.1;
//       currentRow[j] = toSetTop;
//     }
//   }
//
//   for (int j = 0; j < ny; j++) {
//     int i = nx-1;
//     toSetTop = image[j+i*ny];
//     // UPDATE 1 : 3.0 / 5 -> 0.6
//     image[j+i*ny] = image[j+i*ny] * 0.6;
//     if (i > 0)    image[j+i*ny] += currentRow[j] * 0.1;
//     if (j > 0)    image[j+i*ny] += currentRow[j - 1] * 0.1;
//     if (j < ny-1) image[j+i*ny] += image[j+1+i*ny] * 0.1;
//     currentRow[j] = toSetTop;
//   }
// }

void newStencil(const int nx, const int ny, double* restrict image, double* restrict currentRow){
  double toSetTop;

  //DE CE NU FUNCTIONEAZA????

  // toSetTop = image[1+ny];
  // image[1+ny] = image[1+ny] * 0.6 +
  //   (image[1+2*ny]
  //     + image[2+ny]) * 0.1;
  // currentRow[1] = toSetTop;
  // for(int j = 2; j < ny-1; j++){
  //   toSetTop = image[j+ny];
  //   image[j+ny] = image[j+ny] * 0.6 +
  //     (image[j+2*ny]
  //       + currentRow[j-1]
  //       + image[j+1+ny]) * 0.1;
  //   currentRow[j] = toSetTop;
  // }
  for(int j = 0; j < ny; j++)
    currentRow[j] = 0.0;
  for (int i = 1; i < nx-1; i++) {
    for (int j = 1; j < ny-1; j++) {
      toSetTop = image[j+i*ny];
      // UPDATE 1 : 3.0 / 5 -> 0.6
      image[j+i*ny] = image[j+i*ny] * 0.6 +
        (currentRow[j]
          + image[j  +(i+1)*ny]
          + currentRow[j - 1]
          + image[j+1+i*ny]) * 0.1;
      currentRow[j] = toSetTop;
    }
  }

}

void stencil(const int nx, const int ny, double *  image, double *  tmp_image) {
  // UPDATE 2 : change order of fors


  for (int i = 0; i < nx; i++) {
    for (int j = 0; j < ny; j++) {
      // UPDATE 1 : 3.0 / 5 -> 0.6
      tmp_image[j+i*ny] = image[j+i*ny] * 0.6;
      if (i > 0)    tmp_image[j+i*ny] += image[j  +(i-1)*ny] * 0.1;
      if (i < nx-1) tmp_image[j+i*ny] += image[j  +(i+1)*ny] * 0.1;
      if (j > 0)    tmp_image[j+i*ny] += image[j-1+i*ny] * 0.1;
      if (j < ny-1) tmp_image[j+i*ny] += image[j+1+i*ny] * 0.1;
    }
  }
}

// Create the input image
void init_image(const int nx, const int ny, double * image) {
  // Zero everything
  for (int j = 0; j < ny; ++j) {
    for (int i = 0; i < nx; ++i) {
      image[j+i*ny] = 0.0;
    }
  }

  // Checkerboard
  for (int j = 0; j < 8; ++j) {
    for (int i = 0; i < 8; ++i) {
      for (int jj = j*(ny-2)/8; jj < (j+1)*(ny-2)/8; ++jj) {
        for (int ii = i*(nx-2)/8; ii < (i+1)*(nx-2)/8; ++ii) {
          if ((i+j)%2)
          image[(jj+1)+(ii+1)*ny] = 100.0;
        }
      }
    }
  }
}

// Routine to output the image in Netpbm grayscale binary image format
void output_image(const char * file_name, const int nx, const int ny, double *image) {

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
      if (image[j+i*ny] > maximum)
        maximum = image[j+i*ny];
    }
  }
  // Output image, converting to numbers 0-255
  for (int j = 1; j < ny-1; ++j) {
    for (int i = 1; i < nx-1; ++i) {
      fputc((char)(255.0*image[j+i*ny]/maximum), fp);
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
