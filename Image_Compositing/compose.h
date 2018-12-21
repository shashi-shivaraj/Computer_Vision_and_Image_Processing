/**********************************************************************
*  FILE NAME	: compose.h
*
*  DESCRIPTION  : Header file
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  2nd Oct,2018         Shashi Shivaraju        CPSC_6040_HW_03
*                       [C88650674]
***********************************************************************/
#ifndef COMPOSE_HEADER
#define COMPOSE_HEADER

/*header file inclusions*/
#include <iostream>
#include <string>
#include <OpenImageIO/imageio.h>
#include <GL/gl.h>			/*OpenGl & GLUT includes*/
#include <GL/freeglut.h>

/*Open IO namespace*/
OIIO_NAMESPACE_USING

/*macro declarations*/
#undef DEBUG_MODE
#define WINDOW_WIDTH 	600
#define WINDOW_HEIGHT 	600
#define RGBA_CHANNELS	4
#define RGB_CHANNELS	3
#define MONO_CHANNELS 1
#define MAX_IMAGES_SUPPORTED 3

typedef enum __colormode
{
	R_CHANNEL, /*RED*/
	G_CHANNEL, /*GREEN*/
	B_CHANNEL, /*BLUE*/
	RGBA
}colormode;

 struct pixel
{
	unsigned char r, g, b,a; 	/*Structure to store RED,GREEN and BLUE component of a pixel*/
};

/*Structure Declarations*/
typedef struct __imageinfo{
	unsigned int cols;  /*width of image in pixels*/
	unsigned int rows;  /*height of image in pixels*/
	unsigned int channels; /*channels in original image*/
	unsigned char* pixelmap; /*pointers to store pixelmaps*/
	unsigned char* pixelmap_fliped; /*pointers to store pixelmap which is flipped vertically*/
	unsigned char* assoc_color; /*pointer to store associated colors*/
	colormode 	  mode;
}imageinfo;

/*Declaration of function prototypes*/

/*Reshape Callback Routine*/
void handleReshape(int w, int h);
/*Display Callback Routine*/
void drawImage();
/*Keyboard Callback Routine*/
void handleKey(unsigned char key, int x, int y);
/*Function to open image file and store it in a pixel map*/
imageinfo* ReadImage(char* filename);
/*Function to render the image in the window*/
void displayImage();
/*Function to read the displayed pixelmap and store it in a file*/
int WriteImage(std::string outfilename,imageinfo* image);
/*Function to reshape the window accoring to image size*/
void ReshapeWindow();
/*Function to perform composition of two images (foreground over background)*/
imageinfo* ImageComposition(imageinfo* foreground,imageinfo* background);
/*Function to free memory allocations*/
void free_imagememory(imageinfo* image);
/*Function to get associated colors of the image*/
int Get_AssociatedColors(imageinfo* image);
/*Function to vertically flip the image*/
void Image_Flip(imageinfo* image);

#endif /*COMPOSE_HEADER*/
