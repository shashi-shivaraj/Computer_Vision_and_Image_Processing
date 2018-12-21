/**************************************************************************
*  FILE NAME	: manager.h
*
*  DESCRIPTION  : Header file.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  27th Oct,2018        Shashi Shivaraju        CPSC_6040_HW_05
*                       [C88650674]
***************************************************************************/
#ifndef MANAGER_H
#define MANAGER_H

/*header file inclusions*/
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <OpenImageIO/imageio.h>
#include <GL/gl.h>			/*OpenGl & GLUT includes*/
#include <GL/freeglut.h>

#include "matrix.h"

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

typedef enum __warpmode
{
	P_warp,		/*Perspective warp mode = 0 */
	BL_warp,	/*Bilinear warp mode = 1*/
	Twirl_warp	/*Twirl warp mode  = 2*/
}warpmode;


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
		void setFilenames(std::string in,std::string out);
        void drawImage();
        static void handleKey(unsigned char key, int x, int y);
        int ReadImage(std::string filename);
        void ReshapeWindow();
        void handleReshape(int,int);
        static void wrapperForDisplay();
        static void wrapperForhandleReshape(int,int);
        void freeallocations();
        void getInputImageSize(int &c,int &r);
        void ForwardTransform(Matrix3D &M);
        void InverseTransform(Matrix3D invM);
		int WriteImage();
		void BilinearWarp();
		void TwirlWarp(double strength, double cX, double cY);
		int  warpMode;
 

	private:
		static Manager* instance;
		/*constructor*/
		Manager();
		Manager(const Manager& in);
		Manager(std::string infile,std::string outfile);
		/*variables*/
		std::string inputfilename;
		std::string outputfilename;
		pixel** input_image;
		pixel** input_image_flipped;
		pixel** warped_image;
		pixel** warped_image_flipped;
		
		/*Function to vertically flip the image*/
		void Image_Flip(pixel** original,pixel** flipped,int R,int C);
		unsigned int cols;  /*width of input image in pixels*/
		unsigned int rows;  /*height of input image in pixels*/
		
		unsigned int warped_cols;  /*width of output image in pixels*/
		unsigned int warped_rows;  /*height of output image in pixels*/
		Vector2D warped_xycorners[4];
		
		
};

/*Function prototype*/
void InitGlut(int argc, char* argv[]);

#endif /*IMAGE_H*/
