/**************************************************************************
*  FILE NAME	: manager.h
*
*  DESCRIPTION  : Header file.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  20th Nov,2018        Shashi Shivaraju        CPSC_6040_Final_Project
*                       [C88650674]
***************************************************************************/
#ifndef MANAGER_H
#define MANAGER_H

/*header file inclusions*/
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <OpenImageIO/imageio.h>
#include <GL/gl.h>			/*OpenGl & GLUT includes*/
#include <GL/freeglut.h>

#include "matrix.h"			/*used for scaling*/
#include "huffman_header.h" /*used for huffman compression*/


/*Open IO namespace*/
OIIO_NAMESPACE_USING

/*macro declarations*/
#undef DEBUG_MODE

#define WINDOW_WIDTH 	600
#define WINDOW_HEIGHT 	600
#define RGBA_CHANNELS	4
#define RGB_CHANNELS	3
#define MONO_CHANNELS 	1
#define BITS_TO_HIDE	3
#define  ENCRYPTION_HEADER_SIZE 2*RGBA_CHANNELS /*Size in pixels*/

/*enum declaration*/
typedef enum __steganomode
{
	e_default,
	e_compressed
}steganomode;

typedef enum __usagemode
{
	e_hide,
	e_unhide
}usagemode;

typedef enum __filetype
{
	e_cover,
	e_secret,
	e_hidden
}filetype;

/*Structure to store RED,GREEN,BLUE & ALPHA component of a pixel*/
 struct pixel
{
	unsigned char r,g,b,a;
};

/*class declarations*/
class Manager
{
	public:
		static Manager* getInstance();
		void setFilenames(std::string cover,std::string secret,std::string hidden);
		void setUsage(usagemode mode);
		void setMode(steganomode mode);
        void drawImage();
        static void handleKey(unsigned char key, int x, int y);
        int ReadImage(filetype file_mode);
        void ReshapeWindow();
        void handleReshape(int,int);
        static void wrapperForDisplay();
        static void wrapperForhandleReshape(int,int);
        void freeallocations();        
		int WriteImage(filetype file_mode);
		int PerformSteganography();
		int HideImage_DefaultMode();
		int HideImage_CompressionMode();
		int UnHideImage_DefaultMode();
		int UnHideImage_CompressionMode();
		void MergeImages(int config_bits);
		void MergeImages_CompressionMode();

	private:
		static Manager* instance;
		/*constructor*/
		Manager();
		/*variables*/
		std::string coverfilename;
		std::string secretfilename;
		std::string hiddenfilename;
		pixel** cover_image;
		pixel** secret_image;
		pixel** secret_image_flipped;
		pixel** hidden_image;
		pixel** hidden_image_flipped;
		pixel** disp_image;
		
		usagemode usage_mode;
		steganomode stegano_mode;
		
		/*Function to vertically flip the image*/
		void Image_Flip(pixel** original,pixel** flipped,int R,int C);
		void ForwardTransform(Matrix3D &M);
		void InverseTransform(Matrix3D invM);
		void ScaleCoverImage(float scalex,float scaley);
		unsigned int cover_cols;  /*width of cover image in pixels*/
		unsigned int cover_rows;  /*height of cover image in pixels*/
		unsigned int secret_cols;  /*width of secret image in pixels*/
		unsigned int secret_rows;  /*height of secret image in pixels*/
		unsigned int hidden_cols;  /*width of output image in pixels*/
		unsigned int hidden_rows;  /*height of output image in pixels*/
		unsigned int disp_cols;  /*width of display image in pixels*/
		unsigned int disp_rows;  /*height of display image in pixels*/
		
		/*required for scaled cover image*/
		unsigned int scaled_cover_cols;  /*width of new cover image in pixels*/
		unsigned int scaled_cover_rows;  /*height of new cover image in pixels*/
		Vector2D scaled_cover_xycorners[4];
		pixel** scaled_cover_image;
		
		/*required for huffman compressed secret image*/
		unsigned char *compressed_secret_image;
		unsigned int compressed_secret_size;
};

/*Function prototypes*/
void InitGlut(int argc, char* argv[]);

#endif /*MANAGER_H*/
