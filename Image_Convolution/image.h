/**************************************************************************
*  FILE NAME	: image.h
*
*  DESCRIPTION  : Header file.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  12th Oct,2018        Shashi Shivaraju        CPSC_6040_HW_04
*                       [C88650674]
***************************************************************************/
#ifndef IMAGE_H
#define IMAGE_H

/*header file inclusions*/
#include <iostream>
//#include <cstdio>
#include <cmath>
#include <fstream>
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
#define MONO_CHANNELS 	1

typedef enum __colormode
{
	R_CHANNEL, /*RED*/
	G_CHANNEL, /*GREEN*/
	B_CHANNEL, /*BLUE*/
	RGBA
}colormode;


typedef enum __imagemode
{
	ORIGINAL,
	CONVOLUTED
}imagemode;



/*Structure to store RED,GREEN,BLUE & ALPHA component of a pixel*/
 struct pixel
{
	unsigned char r,g,b,a;
};

 struct pixelfloat
{
	float r,g,b,a;
};

class Manager
{
	public:
		static Manager* getInstance();
		void setFilenames(std::string in,std::string out,std::string filter);
        void drawImage();
        static void handleKey(unsigned char key, int x, int y);
        int ReadImage(std::string filename);
        int ReadFilter();
        void ReshapeWindow();
        void handleReshape(int,int);
        static void wrapperForDisplay();
        static void wrapperForhandleReshape(int,int);
        void freeallocations();
        void buildGaborKernel();
        void setGaborSpecs(float the,float sig,float T);
	
	private:
		static Manager* instance;
		/*constructor*/
		Manager();
		Manager(const Manager& in);
		Manager(std::string infile,std::string outfile,std::string filter);
		/*variables*/
		std::string inputfilename;
		std::string outputfilename;
		std::string filterfilename;
		int kernel_size;
		float** kernel_weights;
		float	kernel_scalefactor;
		pixel** input_image;
		pixel** input_image_flipped;
		pixel** convoluted_image;
		pixel** convoluted_image_flipped;
		int WriteImage();
		void ResetImage();
		void ConvolveImage();
		float getGaborWeight(int x,int y);
		/*Function to vertically flip the image*/
		void Image_Flip(pixel** original,pixel** flipped);
		unsigned int cols;  /*width of image in pixels*/
		unsigned int rows;  /*height of image in pixels*/
		/*support for Gabor Filter*/
		float theta; /*in radians*/
		float sigma;
		float period;
		imagemode mode;
};

/*Function prototype*/
void InitGlut(int argc, char* argv[]);

#endif /*IMAGE_H*/
