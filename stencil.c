//
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Define output file name
#define OUTPUT_FILE "stencil.pgm"

void stencil(const int nx, const int ny, float * restrict image, float * restrict tmp_image);
void init_image(const int nx, const int ny, float * image, float * tmp_image);
void output_image(const char * file_name, const int nx, const int ny, float *image);
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
  float *image     = malloc(sizeof(float)*(nx+2)*(ny+2));
  float *tmp_image = malloc(sizeof(float)*(nx+2)*(ny+2));

  // Set the input image
  init_image(nx+2, ny+2, image, tmp_image);

  // Call the stencil kernel
  double tic = wtime();
  for (int t = 0; t < niters; ++t) {
    stencil(nx+2, ny+2, image, tmp_image);
    stencil(nx+2, ny+2, tmp_image, image);
  }
  double toc = wtime();


  // Output
  printf("------------------------------------\n");
  printf(" runtime: %lf s\n", toc-tic);
  printf("------------------------------------\n");

  output_image(OUTPUT_FILE, nx+2, ny+2, image);
  free(image);
}

void stencil(const int nx, const int ny, float * restrict image, float * restrict tmp_image) {
  for( int i = 1; i < nx-1; i++ ) {
    for( int j = 1; j < ny-1; j++ ) {
      tmp_image[ j + i * ny ]  = image[ j +i * ny ] * 0.6f  +
                               ( image[ j - 1 + i * ny ]   +
                                 image[ j + 1 + i * ny ] +
                                 image[ j + (i - 1) * ny ] +
                                 image[ j + (i + 1) * ny ]
                                  ) * 0.1f;
    }
  }
}

// Create the input image
void init_image(const int nx, const int ny, float * image, float * tmp_image) {
  // Zero everything
  for (int j = 0; j < ny; ++j) {
    for (int i = 0; i < nx; ++i) {
      image[j+i*ny] = 0.0;
    }
  }

  // Initialize temporary image with 0s
  for (int i = 0; i < ny; ++i)
    for (int j = 0; j < nx; ++j)
      tmp_image[j+i*ny] = 0.0;

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
void output_image(const char * file_name, const int nx, const int ny, float *image) {

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
  float maximum = 0.0;
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



// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/time.h>
//
// // Define output file name
// #define OUTPUT_FILE "stencil.pgm"
//
// void stencil(const int nx, const int ny, float * restrict image, float *  \
//             top_row, float *  cur_row, float *  bot_row);
// void init_image(const int nx, const int ny, float * image, float *top_row, \
//                 float *cur_row, float *bot_row);
// void output_image(const char * file_name, const int nx, const int ny, float *image);
// double wtime(void);
//
// int main(int argc, char *argv[]) {
//
//     // Check usage
//     if (argc != 4) {
//         fprintf(stderr, "Usage: %s nx ny niters\n", argv[0]);
//         exit(EXIT_FAILURE);
//     }
//
//     // Initiliase problem dimensions from command line arguments
//     int nx = atoi(argv[1]);
//     int ny = atoi(argv[2]);
//     int niters = atoi(argv[3]);
//
//     // Allocate the image
//     float *image   = malloc(sizeof(float)*nx*ny);
//     float *top_row = malloc(sizeof(float)*(nx+2));
//     float *cur_row = malloc(sizeof(float)*(nx+2));
//     float *bot_row = malloc(sizeof(float)*(nx+2));
//
//     // Set the input image
//     init_image(nx, ny, image, top_row, cur_row, bot_row);
//
//     // Call the stencil kernel
//     double tic = wtime();
//     for (int t = 0; t < 2*niters-1; ++t) {
//         stencil(nx, ny, image, top_row, cur_row, bot_row);
//     }
//     double toc = wtime();
//
//     // Output
//     printf("------------------------------------\n");
//     printf(" runtime: %lf s\n", toc-tic);
//     printf("------------------------------------\n");
//
//     output_image(OUTPUT_FILE, nx, ny, image);
//     free(image);
// }
//
// void stencil(const int nx, const int ny, float * restrict image, float *  top_row, \
//             float *  cur_row, float *  bot_row)
// {
//     float *tmp_row;
//     for(int i = 0; i < nx; ++i)
//     {
//         for(int j = 0; j < ny; ++j)
//         {
//             image[ j + i * ny ]  = cur_row[j+1] * 0.6f  +
//             (top_row[j+1] + bot_row[j+1] +
//                 cur_row[j] + cur_row[j+2] ) * 0.1f;
//         }
//
//         tmp_row = top_row;
//         top_row = cur_row;
//         cur_row = bot_row;
//         bot_row = tmp_row;
//
//         if (i < nx-2)
//         {
//             for (int k = 0; k < nx; ++k)
//             {
//                 bot_row[k+1] = image[(i+2)*nx+k];
//             }
//         }
//         else
//         {
//             for (int k = 0; k < nx; ++k)
//             {
//                 bot_row[k+1] = 0;
//             }
//         }
//     }
//
//     for (int i = 1; i < nx+1; ++i)
//     {
//         top_row[i] = 0;
//         cur_row[i] = image[i-1];
//         bot_row[i] = image[nx+i-1];
//     }
// }
//
// // Create the input image
// void init_image(const int nx, const int ny, float *  image, float *top_row, float *cur_row, float *bot_row)
// {
//     // Zero everything
//     for (int j = 0; j < ny; ++j)
//     {
//         for (int i = 0; i < nx; ++i)
//         {
//             image[j+i*ny] = 0.0;
//         }
//     }
//
//     // Checkerboard
//     for (int j = 0; j < 8; ++j)
//     {
//         for (int i = 0; i < 8; ++i)
//         {
//             for (int jj = j*ny/8; jj < (j+1)*ny/8; ++jj)
//             {
//                 for (int ii = i*nx/8; ii < (i+1)*nx/8; ++ii)
//                 {
//                     if ((i+j)%2)
//                     image[jj+ii*ny] = 100.0;
//                 }
//             }
//         }
//     }
//
//     for (int i = 0; i < nx+2; ++i)
//     {
//         top_row[i] = 0;
//         cur_row[i] = 0;
//         bot_row[i] = 0;
//     }
//
//     for (int i = 1; i < nx+1; ++i)
//     {
//         top_row[i] = 0;
//         cur_row[i] = image[i-1];
//         bot_row[i] = image[nx+i-1];
//     }
//
//     // for (int i = 0; i < nx+2; ++i)
//     // {
//     //   printf("%f\n", top_row[i]);
//     // }
//     //
//     // printf("\n");
//     // printf("\n");
//     // printf("\n");
//     //
//     // for (int i = 0; i < nx+2; ++i)
//     // {
//     //   printf("%f\n", cur_row[i]);
//     // }
//     //
//     // printf("\n");
//     // printf("\n");
//     // printf("\n");
//     //
//     // for (int i = 0; i < nx+2; ++i)
//     // {
//     //   printf("%f\n", bot_row[i]);
//     // }
// }
//
// // Routine to output the image in Netpbm grayscale binary image format
// void output_image(const char * file_name, const int nx, const int ny, float *image)
// {
//
//     // Open output file
//     FILE *fp = fopen(file_name, "w");
//     if (!fp)
//     {
//         fprintf(stderr, "Error: Could not open %s\n", OUTPUT_FILE);
//         exit(EXIT_FAILURE);
//     }
//
//     // Ouptut image header
//     fprintf(fp, "P5 %d %d 255\n", nx, ny);
//
//     // Calculate maximum value of image
//     // This is used to rescale the values
//     // to a range of 0-255 for output
//     double maximum = 0.0;
//     for (int j = 0; j < ny; ++j)
//     {
//         for (int i = 0; i < nx; ++i)
//         {
//             if (image[j+i*ny] > maximum)
//             maximum = image[j+i*ny];
//         }
//     }
//
//     // Output image, converting to numbers 0-255
//     for (int j = 0; j < ny; ++j)
//     {
//         for (int i = 0; i < nx; ++i)
//         {
//             fputc((char)(255.0*image[j+i*ny]/maximum), fp);
//         }
//     }
//
//     // Close the file
//     fclose(fp);
//
// }
//
// // Get the current time in seconds since the Epoch
// double wtime(void)
// {
//     struct timeval tv;
//     gettimeofday(&tv, NULL);
//     return tv.tv_sec + tv.tv_usec*1e-6;
// }
