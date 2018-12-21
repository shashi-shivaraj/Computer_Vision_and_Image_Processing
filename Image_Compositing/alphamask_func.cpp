/**********************************************************************
*  FILE NAME	: alphamask_func.cpp
*
*  DESCRIPTION  : Utility functions used to generate an alpha channel 
*				  mask for an image based on its color info.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  1st Oct,2018         Shashi Shivaraju        CPSC_6040_HW_03
*                       [C88650674]
***********************************************************************/
/*Header file  inclusion*/
#include "alphamask.h"

/*Function to convert RGB to HSV values*/
void RGBtoHSV(int r, int g, int b, double &h, double &s, double &v)
{
	double red, green, blue;
	double max, min, delta;
	red = r / 255.0; green = g / 255.0; blue = b / 255.0;
	
	/*r,g,b to 0−1 scale*/
	max = MAXIMUM(red, green, blue);
	min = MINIMUM(red, green, blue);
	
	/*value is maximum of r,g,b*/
	v = max;
	if(max == 0) 
	{
		/*saturation and hue 0 if value is 0*/
		s = 0;
		h = 0;
	}
	else 
	{
		s = (max - min) / max;
		/*saturation is color purity on scale 0 − 1*/
		delta = max - min;
		if(delta == 0) 
		{
			/*hue doesn’t matter if saturation is 0*/
			h = 0;
		} 
		else 
		{
			if(red == max) 
			{
				/*otherwise,determine hue on scale 0−360*/
				h = (green - blue) / delta;
			} 
			else if(green == max)
			{
				h = 2.0 + (blue - red) / delta;
			} 
			else
			{
				/*(blue == max)*/
				h = 4.0 + (red - green) / delta;
			}
			
			h = h * 60.0;
			if (h < 0)
			{
				h = h + 360.0;
			}
		}
	}
}

/*Function to open image file and store it in a pixel map*/
imageinfo* ReadImage(char* filename)
{
	FILE *fp = NULL;
	unsigned char *tempImage = NULL; /*temporary pointer to store the pixel map*/
	int ret = 0; /*variable to check return value*/ 
	int count = 0,i=0,total = 0; /*looping variables*/
	int scanlinesize = 0;
	
	/*open the image file for reading*/
	ImageInput *image = ImageInput::open(filename);
	if(!image)
	{
		   std::cerr << "Could not open image for " << filename << ", error = " << geterror() << std::endl;
		   return NULL;
	}
	
	/*get the image details*/
	const ImageSpec &spec = image->spec();
	
	/*allocate memory to store info of the read image*/
	imageinfo* ImageConfig = (imageinfo*)calloc(1,sizeof(imageinfo));
	if(!ImageConfig)
	{
		 std::cerr << "memory allocation failed "<< std::endl;
		 goto CLEANUP;
	}
	
	ImageConfig->cols = spec.width;
	ImageConfig->rows = spec.height;
	ImageConfig->channels = spec.nchannels;
	
	scanlinesize = spec.width * spec.nchannels * sizeof(unsigned char);
	
#ifdef DEBUG_MODE
	std::cout << filename;
	std::cout <<" with height = " << ImageConfig->rows << " with width = "<< ImageConfig->cols << " and channels = "<< ImageConfig->channels <<std::endl;
#endif /*DEBUG_MODE*/

	/*allocate memory to store the pixels of the image in RGBA format*/
	tempImage = (unsigned char*)calloc(ImageConfig->rows * ImageConfig->cols * ImageConfig->channels,
													sizeof(unsigned char));
	if(!tempImage)
	{
		 std::cerr << "memory allocation failed "<< std::endl;
		 goto CLEANUP;
	}
	
	/*store the image pixels to pixelmap*/
	image->read_image(TypeDesc::UINT8,tempImage);
	
	/*allocate memory to store the pixels of the image in RGBA format*/
	total = ImageConfig->rows * ImageConfig->cols * RGBA_CHANNELS;
	ImageConfig->pixelmap = (unsigned char*)calloc(total,sizeof(unsigned char));
	if(!ImageConfig->pixelmap)
	{
		 std::cerr << "memory allocation failed "<< std::endl;
		 goto CLEANUP;
	}
	
	/*image already in RGBA format*/
	if(RGBA_CHANNELS == ImageConfig->channels)
	{
		/*copy the pixels directly*/
		memcpy(ImageConfig->pixelmap,tempImage,total);
	}
	else if(RGB_CHANNELS == ImageConfig->channels) /*image in RGB format*/
	{
		i = 0;
		count = 0;
		while(count<total)
		{
			ImageConfig->pixelmap[count] = tempImage[i]; /*red*/
			ImageConfig->pixelmap[count+1] = tempImage[i+1];/*green*/
			ImageConfig->pixelmap[count+2] = tempImage[i+2]; /*blue*/
			ImageConfig->pixelmap[count+3] = 255;/*alpha*/
			
			/*increment pointers*/
			count = count + 4;
			i = i + 3;
		}
	}
	else if(MONO_CHANNELS == ImageConfig->channels)/*8 bit mono channel image*/
	{
		i = 0;
		count = 0;
		
		while(count<total)
		{
			ImageConfig->pixelmap[count] = tempImage[i]; /*red*/
			ImageConfig->pixelmap[count+1] = tempImage[i];/*green*/
			ImageConfig->pixelmap[count+2] = tempImage[i]; /*blue*/
			ImageConfig->pixelmap[count+3] = 255;/*alpha*/
			
			/*increment pointers*/
			count = count + 4;
			i++;
		}
	
	}
	
	/*set default color mode*/
	ImageConfig->mode = RGBA;
	
	
	image->close();
    ImageInput::destroy(image);
	
	/*free the allocations*/	 
	if(tempImage)
	{
		free(tempImage);
		tempImage = NULL;
	}
	
	return ImageConfig;
				
	CLEANUP:
		 image->close();
		 ImageInput::destroy(image);
		 
		 if(tempImage)
		 {
			 free(tempImage);
			 tempImage = NULL;
		 }

		 if(ImageConfig)
		 {	
			free_imagememory(ImageConfig);
		 }
		 return NULL;
}

/*
    Routine to write the current framebuffer to an image file
*/
int WriteImage(char *outfilename,imageinfo* image)
{
  /*variable declarations*/
  int width = 0;
  int height = 0;
  unsigned char *pixmap = NULL;

  /* create the oiio file handler for the image*/
  ImageOutput *outfile = ImageOutput::create(outfilename);
  if(!outfile){
    std::cerr << "Could not create output image for " << outfilename << ", error = " << geterror() <<  std::endl;
    return -1;
  }
  
  /*Get the window size*/
  width  = image->cols;
  height = image->rows;
  

  /*The header will indicate an image is of size
   *  width x height and with 4 channels per pixel (RGBA).
   *  All channels will be of type unsigned char*/
  ImageSpec spec(width, height, RGBA_CHANNELS, TypeDesc::UINT8);
  
  /* Open the file for writing the image */ 
  if(!outfile->open(outfilename, spec)){
    std::cerr << "Could not open " << outfilename << ", error = " << geterror() << std::endl;
	ImageOutput::destroy(outfile);
    return -1;
  }

  /* write the image to the file */
  if(!outfile->write_image(TypeDesc::UINT8,image->pixelmap))
  {
    std::cerr << "Could not write image to " << outfilename << ", error = " << geterror() << std::endl;
	ImageOutput::destroy(outfile);	
    return -1;
  }
  else
  {
    std::cout << "Image is stored as "<< outfilename << std::endl;
  }
  
  /* close the image file after the image is written */
  if(!outfile->close())
  {
    std::cerr << "Could not close " << outfilename << ", error = " << geterror() << std::endl;
	ImageOutput::destroy(outfile);
    return -1;
  }
  
  return 0;
}

/*Function to free memory allocations*/
void free_imagememory(imageinfo* image)
{
	if(image)
	{
		if(image->pixelmap)
		{
			free(image->pixelmap);
			image->pixelmap = NULL;
		}
		if(image->hsvmap)
		{
			free(image->hsvmap);
			image->hsvmap = NULL;
		}
				
		free(image);
		image = NULL;
	}
}


/*Function to set the apha channel based on HSV range*/
int ConfigureAlphaMask(imageinfo* ImageConfig,hsvthreshold* hsv_thresholds,int isSSEnabled)
{
	int i = 0,count = 0,total = 0;
	double hue_range = 0,alpha = 0;
	double extended_hue_min = 0;
	
	total = ImageConfig->rows * ImageConfig->cols * RGBA_CHANNELS;
	
	/*allocate memory to store HSV map of the image*/
	ImageConfig->hsvmap = (double*)calloc(ImageConfig->rows * ImageConfig->cols * RGB_CHANNELS,sizeof(double));
	if(!ImageConfig->hsvmap)
	{
		 std::cerr << "memory allocation failed "<< std::endl;
		 return -1;
	}
	
	/*extending the hue range to support full greyscale mask for alpha*/
	extended_hue_min = hsv_thresholds->hue_min-HUE_EXTEND;
	
	/*each pixel represented by RGB vlaue will have its corresponding HSV value*/
	while(count<total)
	{
		RGBtoHSV(ImageConfig->pixelmap[count],		//red
				 ImageConfig->pixelmap[count+1],	//green
				 ImageConfig->pixelmap[count+2],	//blue
				 ImageConfig->hsvmap[i],			//Hue
				 ImageConfig->hsvmap[i+1],			//Saturation
				 ImageConfig->hsvmap[i+2]);         //value
			
		/*determine the value for alpha channel*/	 
		if((extended_hue_min < ImageConfig->hsvmap[i] && 
			ImageConfig->hsvmap[i] < hsv_thresholds->hue_max) &&
		   (hsv_thresholds->saturate_min < ImageConfig->hsvmap[i+1] && 
			ImageConfig->hsvmap[i+1] < hsv_thresholds->saturate_max) &&
		   (hsv_thresholds->value_min < ImageConfig->hsvmap[i+2] && 
		    ImageConfig->hsvmap[i+2] < hsv_thresholds->value_max)
		  )
		{
			if(ImageConfig->hsvmap[i] >= hsv_thresholds->hue_min)
			{
				alpha = 0;
			}
			else /*hue lies between range [extended_hue_min,hsv_thresholds->hue_min]*/
			{
				alpha = (hsv_thresholds->hue_min -ImageConfig->hsvmap[i])*HUE_EXTEND;
			}
			
			ImageConfig->pixelmap[count+3] = (unsigned char)(alpha*255);
		}
		else
		{
			ImageConfig->pixelmap[count+3] = 255;
		}
		
		/*configure green color based on spill supression*/
		if(isSSEnabled)
		{
			ImageConfig->pixelmap[count+1] = MINIMUM(ImageConfig->pixelmap[count],
													 ImageConfig->pixelmap[count+1],
													 ImageConfig->pixelmap[count+2]);
		}
		
		/*increment pointers*/
		count = count + 4;
		i = i+3;
	}
	
	return 0;
}

/*Function to read the config file*/
hsvthreshold* ReadConfig(char* filename)
{
	FILE *fp = NULL;
	hsvthreshold* hsv_thresholds = NULL;
	char key[50];
	double min = 0,max =0;
	
	
	fp = fopen(filename,"r");
	if(!fp)
	{
		std::cout<<"fopen failed for config file\n"<<std::endl;
		return NULL;
	}
	
	/*allocate memeory to store the thresholds*/ 
	hsv_thresholds = (hsvthreshold*)calloc(1,sizeof(hsvthreshold));
	if(!hsv_thresholds)
	{
		std::cout<<"memory allocation failed"<<std::endl;
		return NULL;
	}
	
	while(0<fscanf(fp,"%s %lf %lf",key,&min,&max))
	{
		if(!strcmp(key,"Hue"))
		{
			hsv_thresholds->hue_min = min;
			hsv_thresholds->hue_max = max;
		}
		else if(!strcmp(key,"Saturation"))
		{
			hsv_thresholds->saturate_min = min;
			hsv_thresholds->saturate_max = max;
		}
		else if(!strcmp(key,"Value"))
		{
			hsv_thresholds->value_min = min;
			hsv_thresholds->value_max = max;
		}
	}
	
	if(fp)
		fclose(fp);
	fp = NULL;
	
	return hsv_thresholds;
}
