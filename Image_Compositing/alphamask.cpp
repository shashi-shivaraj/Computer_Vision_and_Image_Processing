/**************************************************************************
*  FILE NAME	: alphamask.cpp
*
*  DESCRIPTION  : Program to generate an alpha channel mask for an image
* 				  based on its color info.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  1st Oct,2018         Shashi Shivaraju        CPSC_6040_HW_03
*                       [C88650674]
***************************************************************************/
/*Header file  inclusion*/
#include "alphamask.h"

/*Main function of the program*/
int main(int argc,char*argv[])
{
	int ret = 0;
	imageinfo* input_image = NULL;
	hsvthreshold *hsv_threshold = NULL;
	char rgbafile[255] = "rgba.png";
	int isSSEnabled = 0; 
	
	/*check for valid command line args*/
	if(4 > argc)
	{
		std::cout<<"usage:./[exe] [input.img] [output.png] [hsv_config.h] [optional flag -ss]"<<std::endl;
		return -1;
	}
	
	/*check if output image has a png extension */
	std::string outfilename(argv[2]);
	if(outfilename.find(".png") == std::string::npos)
	{
		std::cout<<"Invalid output file name\nusage:./[exe] [input.img] [output.png]"<<std::endl;
		return -1;
	}
	
	/*open the input image and store the pixel map */
	input_image = ReadImage(argv[1]);
	if(!input_image)
	{
		std::cout<<"ReadImage failed for input image : "<< argv[1] <<std::endl;
		return -1;
	}
	
	
#ifdef DEBUG_MODE
	/*write the RGBA image to a png file*/
	ret = WriteImage(rgbafile,input_image);
	if(0 != ret)
	{
		std::cout<<"WriteImage failed for output image : "<< argv[2] <<std::endl;
		goto CLEANUP;
	}
#endif /*DEBUG_MODE*/
	
	/*read  the config file to */
	hsv_threshold = ReadConfig(argv[3]);
	if(!hsv_threshold)
	{
		std::cout<<"ReadConfig failed for file : "<< argv[3] <<std::endl;
		ret = -1;
		goto CLEANUP;
	}
	
	/*check if user has requested for spill suppression*/
	if(argc > 4 && !strcmp(argv[4],"-ss"))
	{
		isSSEnabled = 1;
	    std::cout<<"Spill Suppression Enabled"<<std::endl;
	}
	
	/*configure alpha mask of the image based on HSV range*/
	ret = ConfigureAlphaMask(input_image,hsv_threshold,isSSEnabled);
	if(0 != ret)
	{
		std::cout<<"ConfigureAlphaMask failed with : "<< ret <<std::endl;
		goto CLEANUP;
	}
	
	
	
	/*write the image to a png file*/
	ret = WriteImage(argv[2],input_image);
	if(0 != ret)
	{
		std::cout<<"WriteImage failed for output image : "<< argv[2] <<std::endl;
		goto CLEANUP;
	}
	
CLEANUP:	
	if(input_image)
	{
		free_imagememory(input_image);
	}
	
	if(hsv_threshold)
	{
		free(hsv_threshold);
		hsv_threshold = NULL;
	}
	
	return ret;
}
