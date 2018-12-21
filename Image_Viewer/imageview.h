/**********************************************************************
*  FILE NAME	: imageview.h
*
*  DESCRIPTION  : Header file
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  5th Sep,2018         Shashi Shivaraju        CPSC_6040_HW_01
*                       [C88650674]
***********************************************************************/
#ifndef IMAGEVIEW_HEADER
#define IMAGEVIEW_HEADER

/*Header file inclusion*/
#include <iostream>
#include <string>
#include <GL/gl.h>			/*OpenGl & GLUT includes*/
#include <GL/freeglut.h>
#include <OpenImageIO/imageio.h>

/*Open IO namespace*/
OIIO_NAMESPACE_USING

/*Macro declarations*/
#undef DEBUG_MODE
#define WINDOW_WIDTH 	600
#define WINDOW_HEIGHT 	600
#define RGBA_CHANNELS	4
#define RGB_CHANNELS	3
#define MONO_CHANNELS 1
#define MAX_FILE_NAME 1024
#define MAX_IMAGES_SUPPORTED 20

typedef enum __colormode
{
	R_CHANNEL, /*RED*/
	G_CHANNEL, /*GREEN*/
	B_CHANNEL, /*BLUE*/
	RGBA
}colormode;

/*Structure Declarations*/
typedef struct __imageinfo{
	unsigned int cols;  /*width of image in pixels*/
	unsigned int rows;  /*height of image in pixels*/
	unsigned int channels; /*channels in original image*/
	unsigned char* pixelmap; /*pointers to store pixelmaps*/
	unsigned char* rcolormap;
	unsigned char* gcolormap;
	unsigned char* bcolormap;
	colormode 	  mode;
}imageinfo;



/*Declaration of function prototypes*/

/*Reshape Callback Routine*/
void handleReshape(int w, int h);
/*Display Callback Routine*/
void drawImage();
/*Keyboard Callback Routine*/
void handleKey(unsigned char key, int x, int y);
/*Special event Callback Routine*/
void handleSpecialKey(int key,int x,int y);
/*Function to open image file and store it in a pixel map*/
imageinfo* ReadImage(char* filename);
/*Function to render the image in the window*/
void displayImage();
/*Function to read the displayed pixelmap and store it in a file*/
int WriteImage();
/*Function to invert the colors in the pixelmap*/
void invertColors();
/*Function to reshape the window accoring to image size*/
void ReshapeWindow();
/*Function to determine the channel pixels from RGBA pixelmap*/
void UpdateChannelPixelmap();

#endif /*IMAGEVIEW_HEADER*/
