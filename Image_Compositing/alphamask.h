/**********************************************************************
*  FILE NAME	: alphamask.h
*
*  DESCRIPTION  : Header file
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  1st Oct,2018         Shashi Shivaraju        CPSC_6040_HW_03
*                       [C88650674]
***********************************************************************/
#ifndef ALPHAMASK_HEADER
#define ALPHAMASK_HEADER

/*header file inclusions*/
#include <iostream>
#include <string>
#include <OpenImageIO/imageio.h>

/*Open IO namespace*/
OIIO_NAMESPACE_USING

/*macro declarations*/
#undef DEBUG_MODE
#define MAXIMUM(x, y, z) ((x) > (y)? ((x) > (z)? (x) : (z)) : ((y) > (z)? (y) : (z)))
#define MINIMUM(x, y, z) ((x) < (y)? ((x) < (z)? (x) : (z)) : ((y) < (z)? (y) : (z)))

#define RGBA_CHANNELS	4
#define RGB_CHANNELS	3
#define MONO_CHANNELS 	1	

#define HUE_EXTEND		20

/*enum declarations*/
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
	double *hsvmap;
	colormode 	  mode;
}imageinfo;

typedef struct __hsvthreshold{
	double hue_min;
	double hue_max;
	double saturate_min;
	double saturate_max;
	double value_min;
	double value_max;
}hsvthreshold;


/*Function prototypes*/

/*Function to convert RGB to HSV values*/
void RGBtoHSV(int r, int g, int b, double &h, double &s, double &v);

/*Function to open image file and store it in a pixel map*/
imageinfo* ReadImage(char* filename);

/*Function to read the config file*/
hsvthreshold* ReadConfig(char* filename);

/*Function to write a pixelmap into a image file*/
int WriteImage(char *outfilename,imageinfo* image);

/*Function to set the apha channel based on HSV range*/
int ConfigureAlphaMask(imageinfo* image,hsvthreshold* hsv_thresholds,int isSSEnabled);

/*Function to free memory allocations*/
void free_imagememory(imageinfo* image);

#endif /*ALPHAMASK_HEADER*/
