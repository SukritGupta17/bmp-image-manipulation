#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

// method prototypes
void imageload(char *ip_filename, char *op_filename,int flag,float threshold);
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
  float th  = 0.0;
  //printf("%d\n", argc);
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
      imageload(ip_file_n,op_file_n,flg,th);
    }

  }
  // if threshhold function invoked
  if(strcmp(argv[1],"-t") == 0)
  {
    flg = 1;
    th = atof(argv[2]);
    printf("threshold: %f\n",th );
    // with specific output file
    if(argc == 6 && (strcmp(argv[3],"-o") == 0))
    {
      op_file_n = argv[4];
    }
    imageload(ip_file_n,op_file_n,flg,th);
  }

  //printf("%s", file_n );

  return 0;
}

void imageload(char *ip_filename, char *op_filename, int flag, float threshold)
{
  int source_fd, dest_fd;
  unsigned char *source_data, *dest_data;
  size_t filesize;


  // data for source file
  source_fd = open(ip_filename,O_RDONLY);
  printf("FD: %d\n",source_fd );
  if (source_fd == -1) error("problem opening file, use '-h' to check correct usage");
  filesize = lseek(source_fd, 0, SEEK_END);
  source_data = mmap(NULL,filesize,PROT_READ,MAP_PRIVATE,source_fd,0);
  if(source_data == MAP_FAILED) error("mmap problem");
  // ------------------------------------------------------------------
  int Width = source_data[18] | (source_data[19]<<8) |(source_data[20]<<16) | (source_data[21]<<24);
  int height = source_data[22] | (source_data[23]<<8) |(source_data[24]<<16) | (source_data[25]<<24);
  printf("Image Width: %d\nImage Height: %d \n", Width,height);
  //printf("%f\n", ((float)(source_data[54]+source_data[55]+source_data[56]))/(255*3) );
  //printf("%d\n", source_data[57] );
  //-------------------------------------------------------------------
  // check for threshold module
  if(flag == 1)
  {

    //float threshold = 0.5;/printf("%s\n %3s\n","DESCRIPTION:","This program does simple input" );
    printf("%f\n",threshold);
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
      if(pixel > threshold)
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
