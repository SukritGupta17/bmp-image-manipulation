/**
* @author: Sukrit Gupta
* ANU ID: u5900600
* Program Purpose: BMP Image processing and manipulation
*
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

// Macros
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

// function prototypes
void imageload(char *ip_filename, char *op_filename,int flag,float threshold[5]);
void help();
void error(char str[]);
bool isNumber(char number[]);
//---------------------------------------------------------------

int main(int argc, char *argv[]) {
  char *ip_file_n = argv[argc -1];
  char *op_file_n = "out.bmp";
  int flg = 0;
  float th[5]  = {-1.0,0,0,0,0};

  // Check for arguments passed

  if(argc == 1)
  {
    error("no input found, use -h for usage help");
  }
  else if(argc == 2)
  {
    if (strcmp(argv[1],"-h") == 0)
    {
      // HELP MODULE INVOKED
      help();
    }
    else
    {
      // IMAGE DIMENSIONS INVOKED
      imageload(ip_file_n,op_file_n,flg,th);
    }

  }
  else if(argc >= 3 && argc < 9)
  {
    // THRESHOLD MODULE INVOKED
    if(strcmp(argv[1],"-t") == 0)
    {

      if(argc == 4 || (argc == 6 && (strcmp(argv[3],"-o") == 0)))
      {
        flg = 1;
        char * ptr;
        // check for if value passed is correct or not.
        strtof(argv[2],&ptr);
        if(*argv[2] != *ptr) th[0] = atof(argv[2]);
        else error("wrong arguments");

        // with specific output file
        if(argc == 6 ) op_file_n = argv[4];
        // RUN MODULE
        imageload(ip_file_n,op_file_n,flg,th);
      }

      else
      {
        error("wrong usage for threshold! use '-h' to find correct usage");
      }

    }
    // BLEND MODULE INVOKED
    else if(strcmp(argv[1],"-b") == 0)
    {
      if(argc == 5)
      {
        // check for if value passed is correct or not.
        char * ptr;
        strtof(argv[2],&ptr);
        if(*argv[2] != *ptr) th[0] = atof(argv[2]);
        else error("wrong arguments");
      }
      flg = 2;
      // input file
      ip_file_n = argv[argc - 2];
      //output file
      op_file_n = argv[argc - 1];
      //Run Module
      imageload(ip_file_n,op_file_n,flg,th);
    }
    // BRIGHTNESS SATURATION HUE MODULE INVOKED
    else if(strcmp(argv[1],"-bsh") == 0)
    {
      flg = 3;
      if(argc == 6)
      {
        // without specified output file
        op_file_n = "modified.bmp";
        if(isNumber(argv[2]) && isNumber(argv[3]) && isNumber(argv[4]))
        {
          // parsing values for brightness, saturation, hue
          th[0]= atof(argv[2]);
          th[1] = atof(argv[3]);
          th[2] = atof(argv[4]);
        }
        else error("Wrong values passed,use '-h' to find correct usage ");
        // Run module
        imageload(ip_file_n,op_file_n,flg,th);
      }
      // with specific output file
      else if((argc == 8 && (strcmp(argv[5],"-o") == 0)))
      {
        if(isNumber(argv[2]) && isNumber(argv[3]) && isNumber(argv[4]))
        {
          // parsing values for brightness, saturation, hue
          th[0]= atof(argv[2]);
          th[1] = atof(argv[3]);
          th[2] = atof(argv[4]);
        }
        else error("Wrong values passed,use '-h' to find correct usage ");
        // parsing output file name
        op_file_n = argv[argc - 2];
        // Run module
        imageload(ip_file_n,op_file_n,flg,th);

      }

      else
      {
        error("wrong usage! use '-h' to find correct usage");
      }

    }
    // GREYSCALE MODULE INVOKED
    else if(strcmp(argv[1],"-g") == 0)
    {
      flg = 4;
      //with specified output file
      if(argc == 5 && (strcmp(argv[2],"-o") == 0))
      {
        // parse output filename
        op_file_n = argv[argc - 2];
        // Run module
        imageload(ip_file_n,op_file_n,flg,th);
      }
      //without specified output file
      else if(argc == 3)
      {
        // default filename
        op_file_n = "greyscale.bmp";
        // Run Module
        imageload(ip_file_n,op_file_n,flg,th);
      }

      else
      {
        error("wrong usage for greyscale! use '-h' to find correct usage");
      }

    }
    // INVERT MODULE INVOKED
    else if(strcmp(argv[1],"-i") == 0)
    {
      flg = 5;
      //with specified output file
      if(argc == 5 && (strcmp(argv[2],"-o") == 0))
      {
        // parse output filename
        op_file_n = argv[argc - 2];
        // Run Module
        imageload(ip_file_n,op_file_n,flg,th);
      }
      //without specified output file
      else if(argc == 3)
      {
        // default filename
        op_file_n = "invert.bmp";
        // Run Module
        imageload(ip_file_n,op_file_n,flg,th);
      }

      else
      {
        error("wrong usage for invert! use '-h' to find correct usage");
      }

    }
    // COLOR FILTER MODULE INVOKED
    else if(strcmp(argv[1],"-cf") == 0)
    {

      if(argc == 4 || (argc == 6 && (strcmp(argv[3],"-o") == 0)))
      {
        // changing filter wanted into ASCII value
        unsigned char s;
        s = (unsigned char)*argv[2];
        th[0] = (float)s;
        // without specified output
        op_file_n = "colorFilter.bmp";
        flg = 6;
        // with specific output file
        if(argc == 6 ) op_file_n = argv[4];
        // Run Module
        imageload(ip_file_n,op_file_n,flg,th);
      }
      else
      {
        error("wrong usage for threshold! use '-h' to find correct usage");
      }

    }
    else
    {
      error("INVALID ARGUMENTS, use '-h' for help.");
    }
  }
  else
  {
    error("INVALID ARGUMENTS, use '-h' for help.");
  }

  return 0;
}

void imageload(char *ip_filename, char *op_filename, int flag, float threshold[5])
{
  int source_fd, source_fd2, dest_fd;
  int padding;
  unsigned char *source_data,*source_data2, *dest_data;
  size_t filesize;

  /*
  *The approach for mapping data of image file
  *into memory is inspired from
  *bmpshow.c at https://gitlab.cecs.anu.edu.au/comp2300/lecture-example-code
  *by Eric McCreath
  */
  // data for source file
  source_fd = open(ip_filename,O_RDONLY);
  if (source_fd == -1) error("problem opening file, use '-h' to check correct usage");

  filesize = lseek(source_fd, 0, SEEK_END);

  source_data = mmap(NULL,filesize,PROT_READ,MAP_PRIVATE,source_fd,0);
  if(source_data == MAP_FAILED) error("mmap problem");
  // check for image file format (throws error if not BMP)
  if ((source_data[0]) != 66 || (source_data[1] != 77))
  {
    error("Wrong image file format, only takes 24bit .BMP images");
  }
  // ------------------------------------------------------------------
  // converts image dimensions from binary to decimal
  int Width = source_data[18] | (source_data[19]<<8) |(source_data[20]<<16) | (source_data[21]<<24);
  int height = source_data[22] | (source_data[23]<<8) |(source_data[24]<<16) | (source_data[25]<<24);
  // Calculate padding
  padding = (4 - ((Width * 3) % 4)) % 4;
  // Prints Image Dimensions
  printf("Image Width: %d\nImage Height: %d \n", Width,height);
  //-------------------------------------------------------------------
// Start of Feature Modules
  //-------------------------------------------------------------------------
 // THRESHOLD MODULE
  if(flag == 1)
  {
    // data for destination file
    dest_fd = open(op_filename, O_RDWR | O_CREAT, 0666);
    ftruncate(dest_fd, filesize);
    dest_data = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dest_fd, 0);
    // copy file
    memcpy(dest_data, source_data, filesize);
    // find the last pixel in the row of pixels
    int last_pixel = (Width * 3) ;
    // apply threshhold
    for(int i = 54; i < (3 * height * Width) + (padding * height); i += 3)
    {
      // Check for padding and do the relative byte skipping
      if(padding > 0)
      {
        if(i == last_pixel + 54)
        {
          last_pixel = (last_pixel+padding) + (Width * 3);
          i = i + (padding);
        }
      }
      // Convert pixel value into float (0.0 - 1.0)
      float pixel = 0;
      pixel =(float)(dest_data[i]+dest_data[i+1]+dest_data[i+2])/(255*3);
      // Test with threshold value and change image pixels accordingly
      if(pixel > threshold[0])
      {
        dest_data[i] = 255;
        dest_data[i+1] = 255;
        dest_data[i+2] = 255;
      }
      else
      {
        dest_data[i] = 0;
        dest_data[i+1] = 0;
        dest_data[i+2] = 0;
      }
    }
    // delete memory map data
    munmap(dest_data, filesize);
    // close files
    close(dest_fd);
  }
  //---------------------------------------------------------------------
  // IMAGE BLEND module
  else if(flag == 2)
  {
    float blend_threshold = threshold[0];
    // data for second image
    source_fd2 = open(op_filename,O_RDONLY);
    if (source_fd2 == -1) error("problem opening file, use '-h' to check correct usage");

    filesize = lseek(source_fd, 0, SEEK_END);

    source_data2 = mmap(NULL,filesize,PROT_READ,MAP_PRIVATE,source_fd2,0);
    if(source_data2 == MAP_FAILED) error("mmap problem");

    int Width2 = source_data2[18] | (source_data2[19]<<8) |(source_data2[20]<<16) | (source_data2[21]<<24);
    int height2 = source_data2[22] | (source_data2[23]<<8) |(source_data2[24]<<16) | (source_data2[25]<<24);

    // Check if both images are of same dimensions.
    if((Width != Width2) || (height != height2)) error("Both images should be of same Width and height");
    //-------
    // data for output image
    dest_fd = open("blendOut.bmp", O_RDWR | O_CREAT, 0666);
    ftruncate(dest_fd, filesize);
    dest_data = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dest_fd, 0);
    // copy file
    memcpy(dest_data, source_data2, filesize);
    // apply blend
    for(int i = 54; i < (3 * height * Width); i += 3)
    {
      // check if value for blend passed
      if(blend_threshold != -1.0)
      {
        float pixel = 0;
        pixel =(float)(source_data[i]+source_data[i+1]+source_data[i+2])/(255*3);

        if(pixel > blend_threshold)
        {
          dest_data[i] = source_data[i];
          dest_data[i+1] = source_data[i+1];
          dest_data[i+2] = source_data[i+2];
        }
        else
        {
          dest_data[i] = source_data2[i];
          dest_data[i+1] = source_data2[i+1];
          dest_data[i+2] = source_data2[i+2];
        }
      }
      // blend images evenly
      else
      {
        dest_data[i] = (source_data[i] + source_data2[i]) / 2;
        dest_data[i+1] = (source_data[i+1] + source_data2[i+1]) / 2;
        dest_data[i+2] = (source_data[i+2] + source_data2[i+2]) / 2;
      }

    }
    // delete memory map data
    munmap(source_data2, filesize);
    munmap(dest_data, filesize);
    // close files
    close(dest_fd);
    close(source_fd2);
  }
  //----------------------------------------------------------------------
  // BRIGHTNESS SATURATION HUE MODULE
  else if(flag == 3)
  {
    // data for output image
    dest_fd = open(op_filename, O_RDWR | O_CREAT, 0666);
    ftruncate(dest_fd, filesize);
    dest_data = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dest_fd, 0);
    // copy file
    memcpy(dest_data, source_data, filesize);
    // find the last pixel in the row of pixels
    int last_pixel = (Width * 3) ;
    for(int i = 54; i < (3 * height * Width); i += 3)
    {
      // Check for padding and skipping bytes accordingly
      if(padding > 0)
      {
        if(i == last_pixel + 54)
        {
          last_pixel = (last_pixel+padding) + (Width * 3);
          i = i + (padding);
        }
      }
      /*
      *The approach for converting RGB to HSL
      *and back is inspired from
      *Opensource Java Implementation at
      *http://kickjava.com/src/org/eclipse/swt/graphics/RGB.java.htm
      */
      // CONVERT RGB-> HSB
      float r = source_data[i+2] / 255.0;
      float g = source_data[i+1] / 255.0;
      float b = source_data[i] / 255.0;

      float max = MAX(MAX(r, g), b);
      float min = MIN(MIN(r, g), b);
      float delta = max - min;

      float hue = 0;
      float brightness = max;
      float saturation = max == 0 ? 0 : (max - min) / max;
      if (delta != 0) {
          if (r == max) {
              hue = (g - b) / delta;
          } else {
              if (g == max) {
                  hue = 2 + (b - r) / delta;
              } else {
                  hue = 4 + (r - g) / delta;
              }
          }
          hue *= 60;
          if (hue < 0) hue += 360;
       }
       if(i == 54)
       {
         // print out original values of brightness, saturation, hue for one pixel
         printf("Old values:\nhue:%.1f deg saturation:%.0f%% brightness:%.0f%%\n",hue,(saturation*100),(brightness*100) );
       }

       brightness = brightness + (threshold[0]/100);
       // check to make sure value doesn't overflow or underflow
       if(brightness>1.0)
       {
         brightness = 1.0;
       }
       else if(brightness<0.0)
       {
         brightness = 0.0;
       }


       saturation = saturation + (threshold[1]/100);
       // check to make sure value doesn't overflow or underflow
       if(saturation>1.0)
       {
         saturation = 1.0;
       }
       else if(saturation<0.0)
       {
         saturation = 0.0;
       }


       hue = hue + threshold[2];
       // check to make sure value doesn't overflow or underflow
       if(hue>360)
       {
         hue = 360;
       }
       else if(hue<0)
       {
         hue = 0;
       }

    //-------------------------------------------------------------------
    // CONVERT HSB -> RGB
      if (hue < 0 || hue > 360 || saturation < 0 || saturation > 1 ||
             brightness < 0 || brightness > 1)
      {
          error("ERROR_INVALID_ARGUMENT");
      }
      float re, gr, bl;

      if (saturation == 0)
      {
          re = gr = bl = brightness;
      }

      else
      {
          if (hue == 360) hue = 0;
          //hue /= 60;
          int i = (int)(hue/60);
          float f = (hue/60) - i;
          float p = brightness * (1 - saturation);
          float q = brightness * (1 - saturation * f);
          float t = brightness * (1 - saturation * (1 - f));
          switch(i) {
              case 0:
                 re = brightness;
                 gr = t;
                 bl = p;
                 break;
             case 1:
                 re = q;
                 gr = brightness;
                 bl = p;
                 break;
            case 2:
                 re = p;
                 gr = brightness;
                 bl = t;
                 break;
             case 3:
                 re = p;
                 gr = q;
                 bl = brightness;
                 break;
             case 4:
                 re = t;
                 gr = p;
                 bl = brightness;
                 break;
             case 5:
             default:
                 re = brightness;
                 gr = p;
                 bl = q;
                 break;
          }
        }

        if(i == 54)
        {
          // Print the modified values for the same pixel
          printf("New Values:\nhue:%.1f deg saturation:%.0f%% brightness:%.0f%%\n",hue,(saturation*100),(brightness*100) );
        }

        // writing new RGB values into the output file
         dest_data[i+2] = (int)(re * 255 + 0.5);
         dest_data[i+1] = (int)(gr * 255 + 0.5);
         dest_data[i] = (int)(bl * 255 + 0.5);

    }
    // delete memory map data
    munmap(dest_data, filesize);
    // close files
    close(dest_fd);
  }
  //-----------------------------------------------------------------------
  // GRAYSCALE MODULE
  else if(flag == 4)
  {
    // data for output image
    dest_fd = open(op_filename, O_RDWR | O_CREAT, 0666);
    ftruncate(dest_fd, filesize);
    dest_data = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dest_fd, 0);
    // copy file
    memcpy(dest_data, source_data, filesize);
    // find the last pixel in the row of pixels
    int last_pixel = (Width * 3) ;
    for(int i = 54; i < (3 * height * Width) + (padding * height); i += 3)
    {
      // Check for padding and skipping bytes accordingly
      if(padding > 0)
      {
        if(i == last_pixel + 54)
        {
          last_pixel = (last_pixel+padding) + (Width * 3);
          i = i + (padding);
        }
      }

      float pixel = 0;
      pixel =(dest_data[i]+dest_data[i+1]+dest_data[i+2])/3;

      // making output image grayscale
      dest_data[i] = pixel;
      dest_data[i+1] = pixel;
      dest_data[i+2] = pixel;
    }
    // delete memory map data
    munmap(dest_data, filesize);
    // close files
    close(dest_fd);

  }
  //--------------------------------------------------------------------------
  // INVERT COLOR MODULE
  else if(flag == 5)
  {
    // data for output image
    dest_fd = open(op_filename, O_RDWR | O_CREAT, 0666);
    ftruncate(dest_fd, filesize);
    dest_data = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dest_fd, 0);
    // copy file
    memcpy(dest_data, source_data, filesize);
    // find the last pixel in the row of pixels
    int last_pixel = (Width * 3) ;
    for(int i = 54; i < (3 * height * Width) + (padding * height); i += 3)
    {
      // check for padding and skipping bytes accordingly
      if(padding > 0)
      {
        if(i == last_pixel + 54)
        {
          last_pixel = (last_pixel+padding) + (Width * 3);
          i = i + (padding);
        }
      }

      // inverting color and writing data into output file
      dest_data[i] = 255 - source_data[i];
      dest_data[i+1] = 255 - source_data[i+1];
      dest_data[i+2] = 255 - source_data[i+2];

    }
    // delete memory map data
    munmap(dest_data, filesize);
    // close files
    close(dest_fd);

  }
  //-----------------------------------------------------------------------
  // COLOR FILTER MODULE
  else if(flag == 6)
  {
    // data for output image
    dest_fd = open(op_filename, O_RDWR | O_CREAT, 0666);
    ftruncate(dest_fd, filesize);
    dest_data = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dest_fd, 0);
    // copy file
    memcpy(dest_data, source_data, filesize);
    int last_pixel = (Width * 3) ;
    for(int i = 54; i < (3 * height * Width) + (padding * height); i += 3)
    {
      // check for padding and skipping bytes accordingly
      if(padding > 0)
      {
        if(i == last_pixel + 54)
        {
          last_pixel = (last_pixel+padding) + (Width * 3);
          i = i + (padding);
        }
      }
      // check for red filter
      if(threshold[0] == 114.0)
      {
        dest_data[i] = 0;
        dest_data[i+1] = 0;
        dest_data[i+2] = source_data[i+2];
      }
      // check for green filter
      else if(threshold[0] == 103.0)
      {
        dest_data[i] = 0;
        dest_data[i+1] = source_data[i+1];
        dest_data[i+2] = 0;
      }
      // check for blue filter
      else if(threshold[0] == 98.0)
      {
        dest_data[i] = source_data[i];
        dest_data[i+1] = 0;
        dest_data[i+2] = 0;
      }
      else error("wrong filter value passed, use '-h' to find correct usage" );


    }
    // delete memory map data
    munmap(dest_data, filesize);
    // close files
    close(dest_fd);

  }
  // features end
  //---------------------------------------------------------------------------

// delete memory map data
  munmap(source_data, filesize);
// close files
  close(source_fd);
 }
 //--------------------------------------------------
/*
* Help module starts here
*/
 void help()
 {
    int c;
    FILE *file;
    file = fopen("help.txt", "r");
    if (file)
    {
        c = getc(file);
        while (!feof(file))
        {
          putchar(c);
          c = getc(file);

        }
        fclose(file);
    }
  }
  /*
  * error check module starts here
  */
  void error(char str[])
  {
     printf ("Error : %s\n",str);
     exit(1);
  }
  /*
  * argument check module for "-bsh" flag
  */
  bool isNumber(char number[])
  {
      int i = 0;

      //checking for negative numbers
      if (number[0] == '-')
          i = 1;
      for (; number[i] != 0; i++)
      {
          //if (number[i] > '9' || number[i] < '0')
          if (!isdigit(number[i]))
              return false;
      }
      return true;
  }

 /* ----------------------------------------------------------------------*/
 // Program End
