# Assignnment 1

* **Name** : Sukrit Gupta
* **Uni ID** : u5900600

## Summary

 *bmpedit* is a image processing and image manipulation program that takes "*.bmp" images as input from the command line
 and processes the image according to the arguments passed by the user during execution.

### The program currently includes the following features :
* #### Image Dimensions:
    When the following command is run :
    ```sh
    $ ./bmpedit input_filename.bmp
    ```
    The program outputs the dimensions of the input file:
    ```sh
     Image Width: 640
     Image Height: 480
    ```
    The above output is achieved by mapping the data of the image using the mmap function present in the
    `#include <sys/mman.h>` library. The below mentioned way makes a private map of the data of the input file.
    Its loaded only for the purpose of reading the file`MAP_PRIVATE` & `PROT_READ`(arguments passed for this purpose), so     as to keep the original image intact.[1]

    ```c
    mmap(NULL,filesize,PROT_READ,MAP_PRIVATE,source_fd,0);
    ```
    The device independent bitmap (DIB) file format or simply a "bitmap(*.bmp)" contains information about the image file     inside the two headers[2]:
    * BMP Header -: 14bytes
    * DIB Header -: 40 bytes

    The Height and Width of the image is stored inside the DIB Header as an int, which takes 4 bytes of space. They are       stored at the offset of: Width: byte 18 and Height: byte 22.
    ```c
    (source_data[18])| (source_data[19]<<8) |(source_data[20]<<16) | (source_data[21]<<24)
    ```
    The above code line was used to extract the data stores in those bytes and convert them back to "Decimal Number           System" base 10.[1]

* #### Threshold Filter :
    When the following command is executed :
    ```sh
    $ ./bmpedit -t 0.5 -o output_filename.bmp input_filename.bmp
    ```
    **OR**

    ```sh
    $ ./bmpedit -t 0.5 input_filename.bmp
    ```
    The Threshold filter is invoked by the "-t" flag in the command arguments and the value after that is taken as the       threshold, it is applied to the input file and the resulting image is saved in the output file named in the command      line argument after the flag "-o" is encountered . If "-o" is not used, a default "out.bmp" file is used instead.        The output image is processed using mmap function again, but this time the memory map is set to `MAP_SHARED` and the     file is `PROT_READ | PROT_WRITE` so as to have access for reading and writing to the file.
    ```c
    mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dest_fd, 0)
    ```
    The pixels of the image are stored in a pixel array that starts immediately after the DIB header i.e. 14 + 40 bytes is     the offset to access the pixel array. A single Pixel is made up of RGB which each individually takes 1 byte of space      and their value ranges from 0-255. In the pixel array every three bytes make up a pixel, the first byte corresponds      to the value of 'B', the second to 'G' and third to 'R'.  To convert the pixel color from an integer between 0 and        2^((bits per pixel/3) - 1, to a floating point number between 0 and 1, to make the pixel comparable to the threshold      value passed by the user, the following math formula was used :
    ` (float)(adding the three bytes of a pixel) / (255*3)`
    this gave a value that can be compared with threshold value passed and each pixel is manipulated accordingly, pixel       higher than threshold value is turned white and pixel lower than threshold value is turned black.
    ##### Sample output:




* #### Picture Blend Feature
    When the following command is executed :
    ```sh
    $ ./bmpedit -b 0.3 input_filename1.bmp input_filename2.bmp
    ```
    **OR**

    ```sh
    $ ./bmpedit -b input_filename1.bmp input_filename2.bmp
    ```
    The picture blend feature is invoked when the "-b" flag is encountered in the command line execution of the program.
    It takes two images as input and a blend coefficient(0.0 - 1.0) (if provided). The images are loaded using the same      MMAP function and the image dimensions of both the images are checked to see if they are equal. The Image Blend only     works for images of same dimensions and throws an error if they are not (Limitation).
    If the blend coefficient is passed, the process used for blending is to take the input image2 as the background image     and the input image1 is blended on top of it depending on the amount of blend coefficient passed. This is done by        converting the value of a pixel into a floating point between 0.0 - 1.0 and compared with the blend coefficient.
    If no blend coefficient is passed both the images are blended in 50 - 50. This process is carried out by averaging       the pixels from both the images and using that average as the value for the output image file pixel `out_pixel =         (ip1_pixel + ip2_pixel) / 2;`. This technique results in a evenly blended image. The output image file name is fixed     to "BlendOut.bmp".
     ##### Sample output:



* #### Adjusting "Brightness" "Saturation" "Hue"
    The following command can be used to adjust the brightness , saturation and hue of any 24bit bmp image.
    ```sh
    $ ./bmpedit -bsh 23 5 0 -o output_filename.bmp input_filename.bmp
    ```
    **OR**

    ```sh
    $ ./bmpedit -bsh -10 30 5 input_filename.bmp
    ```
    When the  "-bsh" flag is encountered, the brightness, saturation, hue module is invoked in the code. The three values     passed after the flag correspond to b for brightness, s for saturation and h for hue, respectively. These values are     from 0 - 100 and represent increase in original values to be made. The value for brightness and saturation is in         percentage and the value for hue is in degrees.[3]
    The mechanism used to achieve this feature was to covert the RGB values for every pixel into the HSL values i.e. Hue     Saturation and Luminance. The math formula to do the conversions to HSL and back to RGB and coding it into c was          achieved by taking help from various resources available on the internet like wikipedia[4], stackoverflow[5],            kickjava[6], niwa[7].
    If a '-o' flag is encountered in the command line arguments the file name next to it is used for the output file,
    otherwise a default file name "modified.bmp" is used for the output image.
    The arguments for brightness, saturation and hue work together and individually if the argument value passed is 0 no     changes are made for that part. You can increase and decrease the values accordingly.
    The first example code increases the brightness by 23%, the saturation by 5% and no change is made for hue.
    In the second example brightness is decreased by 10% saturation increased by 30% and hue is moved 5 degrees.
    It also prints out the old values of brightness, saturation and hue and the new values after the process is done for     one pixel in the image to provide the user with information of the modifications made to the values of the image.

    ##### Sample output:

* #### Grayscale Filter
    The following command invokes the grayscale filter on the input image.
    ```sh
    $ ./bmpedit -g -o output_filename.bmp input_filename.bmp
    ```
    **OR**

    ```sh
    $ ./bmpedit -g input_filename.bmp
    ```
    The "-g" flag invokes the grayscale filter in the program, it takes the input image provided in the command line         argument and maps the image data using the MMAP function as described above. The math for the grayscale to be            achieved is simple and the knowledge was acquired from wikipedia[8]. To achieve grayscale for any 24bit .bmp image       file you take the RGB value, add them together and divide them by 3, essentially getting the average value and set       that pixel to the value obtained. `pixel =(value for R + value for G + value for B)/3;` Repeating this process for        every pixel in the image will output a grayscale image.

    ##### Sample output:

* #### Help Module
    The following command invokes the help for the bmpedit program:
    ```sh
    $ ./bmpedit -h
    ```
    The "-h" flag invokes the help module that prints out the usage help for the program, with explanations on how           different modules can be invoked and used. The help text is saved in a help.txt file for the ease of editing at later     stages. The program just opens the file and prints out the content on the terminal when help module is invoked.

## Way to Compile :
* __For latest version of GCC__
    ```sh
    $ gcc -Wall -o bmpedit bmpedit.c
    ```
* __For older versions of GCC__
    ```sh
    $ gcc -std=c99 -o bmpedit bmpedit.c
    ```
## Testing done :

Each module was tested with different 24 bit .bmp images, various bugs were removed, many of the images used for testing are included in repository. Usage of printf statements at various places to check if the modules were being invoked aptly. printing out the image header to find the information required. printing out the image data bytes for the pixel array to check if the loops to manipulate pixels were going to the end of the file.
To make the program robust: tested it with various wrong arguments i.e. passing a string where a number value is expected, images with varying dimensions (not modulo 4) and making the program able to handle padding in images.

## Optional Extensions (deserving of higher marks):

- Brightness Saturation and Hue modifier:
    - Can be used to adjust brightness Saturation and Hue together or individually by setting the ones not being changed     to 0.
- Grayscale converter:
    - Converts any 24bit BMP image to Grayscale padding or no padding i.e. Image dimensions don't matter.
- Image Blend :
    - The usage explained above
    - can be used to blend 2 images together
## Limitations and Future Improvements:

- Parsing command line arguments in a different order aren't handled.
    - implement a search algorithm to remedy the above limitation.
- Any other *.bmp file that isn't 24bit BMP is not handled by the program.
    - be able to handle header formats of other BMP type images.
- Only a default output image for the blend module and can only handle two images of same dimensions.
    - add the '-o' flag to this module to be able to receive output file names from the user.
    - be able to blend two images with varying sizes and dimensions.
- Add other useful extensions like:
    - Cropping images
    - Resizing
    - Rotation by degree
    - Blur (various kinds)
- Add ways to handle other image file formats and compressions used by these formats like JPEG, PNG etc.


#### References:
- [1] bmpshow.c at https://gitlab.cecs.anu.edu.au/comp2300/lecture-example-code by Eric
McCreath
- [2] BMP File Format : https://en.wikipedia.org/wiki/BMP_file_format
- [3] RGB and HSL Calculator: http://serennu.com/colour/hsltorgb.php
- [4] HSL and HSV math: https://en.wikipedia.org/wiki/HSL_and_HSV
- [5] RGB -> HSL converter code: http://stackoverflow.com/questions/2353211/hsl-to-rgb-color-conversion
- [6] Opensource Java Implementation: http://kickjava.com/src/org/eclipse/swt/graphics/RGB.java.htm
- [7] Math formulas for conversion: http://www.niwa.nu/2013/05/math-behind-colorspace-conversions-rgb-hsl/
- [8] GrayScale : https://en.wikipedia.org/wiki/Luma_%28video%29 , https://en.wikipedia.org/wiki/Grayscale
