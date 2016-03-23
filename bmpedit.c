#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

// method prototypes
void imageload(char *ip_filename, char *op_filename,int flag,int miniFlag,float threshold[5]);
void help();
void error(char str[]) {
   printf ("Error : %s\n",str);
   exit(1);
}
//---------------------------------------------------------------

int main(int argc, char *argv[]) {
  char *ip_file_n = argv[argc -1];
  char *op_file_n = "out.bmp";
  int flg = 0;
  int mf = 0;
  float th[5]  = {-1.0,0,0,0,0};
  //printf("%d\n", argc);
  if(argc == 1)
  {
    error("no input found, use -h for usage help");
  }
  if(argc == 2)
  {
    if (strcmp(argv[1],"-h") == 0)
    {
      //ip_file_n =
      help();
    }
    else
    {
      //ip_file_n = argv[1];
      imageload(ip_file_n,op_file_n,flg,mf,th);
    }

  }
  // if threshhold function invoked
  if(strcmp(argv[1],"-t") == 0)
  {
    flg = 1;
    th[0] = atof(argv[2]);
    //printf("threshold: %f\n",th );
    // with specific output file
    if(argc == 6 && (strcmp(argv[3],"-o") == 0))
    {
      op_file_n = argv[4];
    }
    imageload(ip_file_n,op_file_n,flg,mf,th);
  }
  else if(strcmp(argv[1],"-b") == 0)
  {
    if(argc == 5)
    {
      th[0] = atof(argv[2]);
    }
    flg = 2;
    ip_file_n = argv[argc - 2];
    op_file_n = argv[argc - 1];

    imageload(ip_file_n,op_file_n,flg,mf,th);
  }
  else if(strcmp(argv[1],"-br") == 0 || strcmp(argv[1],"-sa") == 0  || strcmp(argv[1],"-hu") == 0 || strcmp(argv[1],"-bsh") == 0)
  {
    flg = 3;
    th[0] = atof(argv[2]);
    if(strcmp(argv[1],"-sa") == 0) mf = 1;
    else if(strcmp(argv[1],"-hu") == 0) mf = 2;
    else if(strcmp(argv[1],"-bsh") == 0)
    {
      th[1] = atof(argv[3]);
      th[2] = atof(argv[4]);
      mf = 3;
    }
    //printf("threshold: %f\n",th );
    // with specific output file
    if((argc == 6 && (strcmp(argv[3],"-o") == 0)) || (argc == 8 && (strcmp(argv[5],"-o") == 0)))
    {
      op_file_n = argv[argc - 2];
    }
    else
    {
      error("wrong usage! use '-h' to find correct usage");
    }
    imageload(ip_file_n,op_file_n,flg,mf,th);
  }

  //printf("%s", file_n );

  return 0;
}

void imageload(char *ip_filename, char *op_filename, int flag,int miniFlag, float threshold[5])
{
  int source_fd,source_fd2, dest_fd;
  unsigned char *source_data,*source_data2, *dest_data;
  size_t filesize;


  // data for source file
  source_fd = open(ip_filename,O_RDONLY);
  //printf("FD: %d\n",source_fd );
  if (source_fd == -1) error("problem opening file, use '-h' to check correct usage");
  filesize = lseek(source_fd, 0, SEEK_END);
  source_data = mmap(NULL,filesize,PROT_READ,MAP_PRIVATE,source_fd,0);
  if(source_data == MAP_FAILED) error("mmap problem");
  // ------------------------------------------------------------------
  // Prints Image Dimensions
  int Width = source_data[18] | (source_data[19]<<8) |(source_data[20]<<16) | (source_data[21]<<24);
  int height = source_data[22] | (source_data[23]<<8) |(source_data[24]<<16) | (source_data[25]<<24);
  printf("Image Width: %d\nImage Height: %d \n", Width,height);
  //printf("%f\n",(0.2126*(float)source_data[56] ));
  //printf("%f\n", ((0.2126*(float)source_data[56] + 0.7152*(float)source_data[55] + 0.0722*(float)source_data[54])));
  //printf("%d\n", source_data[57] );
  //-------------------------------------------------------------------

 //-------------------------------------------------------------------------
 // THRESHOLD MODULE
  if(flag == 1)
  {

    //float threshold = 0.5;/printf("%s\n %3s\n","DESCRIPTION:","This program does simple input" );
    printf("%f\n",threshold[0]);
    // data for destination file
    dest_fd = open(op_filename, O_RDWR | O_CREAT, 0666);
    ftruncate(dest_fd, filesize);
    dest_data = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dest_fd, 0);
    // copy file
    memcpy(dest_data, source_data, filesize);
    // apply threshhold
    for(int i = 54; i < (3 * height * Width); i += 3 )
    {
      float pixel = 0;
      pixel =(float)(dest_data[i]+dest_data[i+1]+dest_data[i+2])/(255*3);
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
    //printf("FD: %d\n",source_fd2 );
    if (source_fd2 == -1) error("problem opening file, use '-h' to check correct usage");
    filesize = lseek(source_fd, 0, SEEK_END);
    source_data2 = mmap(NULL,filesize,PROT_READ,MAP_PRIVATE,source_fd2,0);
    if(source_data2 == MAP_FAILED) error("mmap problem");
    //-------
    // data for output image
    dest_fd = open("blendOut.bmp", O_RDWR | O_CREAT, 0666);
    ftruncate(dest_fd, filesize);
    dest_data = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dest_fd, 0);
    // copy file
    memcpy(dest_data, source_data2, filesize);
    // apply blend
    for(int i = 54; i < (3 * height * Width); i += 3 )
    {
      if(blend_threshold != -1.0)
      {
        float pixel = 0;
        //float blend_threshold = 0.5;
        pixel =(float)(source_data[i]+source_data[i+1]+source_data[i+2])/(255*3);
        //pixel2 =(float)(source_data2[i]+source_data2[i+1]+source_data2[i+2])/(255*3);

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
      else
      {
        dest_data[i] = (source_data[i] + source_data2[i]) / 2;
        dest_data[i+1] = (source_data[i+1] + source_data2[i+1]) / 2;
        dest_data[i+2] = (source_data[i+2] + source_data2[i+2]) / 2;
      }



    }


  }
  //----------------------------------------------------------------------
  // BRIGHTNESS SATURATION HUE MODULE
  else if(flag == 3)
  {

    dest_fd = open(op_filename, O_RDWR | O_CREAT, 0666);
    ftruncate(dest_fd, filesize);
    dest_data = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dest_fd, 0);
    // copy file
    memcpy(dest_data, source_data, filesize);
    for(int i = 54; i < (3 * height * Width); i += 3 )
    {
      // CONVERT RGB-> HSB
      float r = source_data[i+2] / 255.0;
      float g = source_data[i+1] / 255.0;
      float b = source_data[i] / 255.0;
    //   if(i == 54)
    //   {
    //     printf("r:%d g:%d b:%d\n",source_data[i+2],source_data[i+1],source_data[i] );
    //     printf("r:%f g:%f b:%f\n",r,g,b );
    //  }
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
         printf("Old values:\nhue:%.1f deg saturation:%.0f%% brightness:%.0f%%\n",hue,(saturation*100),(brightness*100) );

       }
       if(miniFlag == 0 || miniFlag == 3)
       {
         brightness = brightness + (threshold[0]/100);

         if(brightness>1.0)
         {
           brightness = 1.0;
         }
         else if(brightness<0.0)
         {
           brightness = 0.0;
         }
       }
       if(miniFlag == 1 || miniFlag == 3)
       {
         if(miniFlag == 1) saturation = saturation + (threshold[0]/100);
         else if(miniFlag == 3) saturation = saturation + (threshold[1]/100);
         if(saturation>1.0)
         {
           saturation = 1.0;
         }
         else if(saturation<0.0)
         {
           saturation = 0.0;
         }
       }
       if(miniFlag == 2 || miniFlag == 3)
       {
         if(miniFlag == 2) hue = hue + threshold[0];
         else if(miniFlag == 3) hue = hue + threshold[2];
         if(hue>360)
         {
           hue = 360;
         }
         else if(hue<0)
         {
           hue = 0;
         }
       }



       //printf("new B:%f\n",brightness );

    //-------------------------------------------------------------------
    // CONVERT HSB -> RGB
      if (hue < 0 || hue > 360 || saturation < 0 || saturation > 1 ||
             brightness < 0 || brightness > 1) {
             error("ERROR_INVALID_ARGUMENT");
         }
      float re, gr, bl;
      if (saturation == 0) {
          re = gr = bl = brightness;
      } else {
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
          printf("New Values:\nhue:%.1f deg saturation:%.0f%% brightness:%.0f%%\n",hue,(saturation*100),(brightness*100) );
        }


         dest_data[i+2] = (int)(re * 255 + 0.5);
         dest_data[i+1] = (int)(gr * 255 + 0.5);
         dest_data[i] = (int)(bl * 255 + 0.5);
         //printf("r:%d g:%d b:%d\n",source_data[i+2],source_data[i+1],source_data[i] );

    }


  }
// delete memory map data
  munmap(source_data, filesize);
  //munmap(dest_data, filesize);
// close files
  close(source_fd);
 }
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
        while (!feof(file))
        {
          c = getc(file);
          putchar(c);
        }
        fclose(file);
    }
 }

 /* ----------------------------------------------------------------------*/
