/**********************************************************************
*  FILE NAME	: compose_func.cpp
*
*  DESCRIPTION  : Utility functions used to perform composition of 
*				  two images.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  1st Oct,2018         Shashi Shivaraju        CPSC_6040_HW_03
*                       [C88650674]
***********************************************************************/
/*Header file  inclusion*/
#include "compose.h"

/*Global Declarations */
imageinfo *gImageInfo[MAX_IMAGES_SUPPORTED] = {0}; /*array of pointers to store image data*/
static int ImageCount = -1;			/* varibale to count images loaded currently*/
static int CurrentImageIndex = 0;  /* varibale to track current image displayed*/

/* 
	Reshape Callback Routine:
*/
void handleReshape(int w, int h)
{
	// set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// define the drawing coordinate system on the viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);// sets up a 2D orthographic viewing region
}

/*
   Display Callback Routine:clear the screen and draw the pixelmap
   This routine is called every time the window on the screen needs
   to be redrawn, like if the window is iconized and then reopened
   by the user, and when the window is first created. It is also
   called whenever the program calls glutPostRedisplay()
*/
void drawImage()
{
	if(gImageInfo[CurrentImageIndex])
	{
		if(gImageInfo[CurrentImageIndex]->pixelmap_fliped)
		{
			/* set the window backgroung to black*/
			glClearColor(0,0,0,0);
			glClear(GL_COLOR_BUFFER_BIT);
			glRasterPos2i(0,0);  
			/*Draw the pixels in the window*/
			if(RGBA == gImageInfo[CurrentImageIndex]->mode)
			{
				glDrawPixels(gImageInfo[CurrentImageIndex]->cols,
							gImageInfo[CurrentImageIndex]->rows,
							GL_RGBA,GL_UNSIGNED_BYTE,gImageInfo[CurrentImageIndex]->pixelmap_fliped);
			}
			
			glutReshapeWindow(gImageInfo[CurrentImageIndex]->cols,gImageInfo[CurrentImageIndex]->rows);
			// flush the OpenGL pipeline to the viewport
			glFlush();
		}
	}
}

/*function to set the window size to fit the image size*/
void ReshapeWindow()
{
	if(gImageInfo[CurrentImageIndex])
	{
		if(gImageInfo[CurrentImageIndex]->pixelmap)
		{
			glutReshapeWindow(gImageInfo[CurrentImageIndex]->cols,gImageInfo[CurrentImageIndex]->rows);
			// flush the OpenGL pipeline to the viewport
			glFlush();
		}
	}
}

/*
  Keyboard Callback Routine: This routine is called every time a key is pressed on the keyboard
*/
void handleKey(unsigned char key, int x, int y)
{
	int ret = 0,i = 0;

  switch(key){
	/*write the display to a file*/
	case 'w':
	case 'W':
	{
		if(gImageInfo[CurrentImageIndex])
		{
			std::string outfilename;
			/* get a filename for the image */
			std::cout << "enter output image filename: ";
			std::cin >> outfilename;
  
			ret = WriteImage(outfilename,gImageInfo[CurrentImageIndex]);
			if(0 != ret)
			{
				std::cerr<< "WriteImage() function failed " <<std::endl;
			}
		}
	}
	break;
	/*exit the program when user presses q,Q or esc*/  
    case 'q':		
    case 'Q':
    case 27:
    {
		/*need to deallocate resources before exiting*/
		for(i = 0;i<ImageCount;i++)
		{
			if(gImageInfo[i])
			{
				 free_imagememory(gImageInfo[i]);
			}
		}
		ImageCount = -1;
		CurrentImageIndex = 0;
		exit(0);
	}
	break;
      
    default:
    		// not a valid key -- just ignore it
      return;
  }
}

/*Function to open image file and store it in a pixel map*/
imageinfo* ReadImage(char* filename)
{
	FILE *fp = NULL;
	unsigned char *tempImage = NULL; /*temporary pointer to store the pixel map*/
	int ret = 0; /*variable to check return value*/ 
	int count = 0,i=0,total = 0; /*looping variables*/
	
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
	
	
#ifdef DEBUG_MODE
	std::cout << filename <<std::endl;
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
	
	/*Flip an image vertically upon reading, by using negative y stride.
	 * since OpenGL render from bottom left corner of raster*/
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
	
	/*update the global pointer*/
	ImageCount++;
	gImageInfo[ImageCount] = ImageConfig;
	
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
			free(ImageConfig->pixelmap);
			ImageConfig->pixelmap = NULL;
			free(ImageConfig);
			ImageConfig = NULL;
		 }
		 return NULL;
}

/*
    Routine to write the current framebuffer to an image file
*/
int WriteImage(std::string outfilename,imageinfo* image)
{
  	
  /*variable declarations*/
  int width = image->cols;
  int height = image->rows;

  /* create the oiio file handler for the image*/
  ImageOutput *outfile = ImageOutput::create(outfilename);
  if(!outfile){
    std::cerr << "Could not create output image for " << outfilename << ", error = " << geterror() << std::endl;
    return -1;
  }

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

/*Function to perform composition of two images (foreground over background)*/
imageinfo* ImageComposition(imageinfo* foreground,imageinfo* background)
{
	imageinfo* OutputImage = NULL;
	int ret = 0;
	int count = 0,total = 0,j = 0;
	double alpha = 0;
	
	/*allocate memory to store info of the read image*/
	OutputImage = (imageinfo*)calloc(1,sizeof(imageinfo));
	if(!OutputImage)
	{
		 std::cerr << "memory allocation failed "<< std::endl;
		 return NULL;
	}
	
	/*For composition background image should have greater or equal dimension
	 * when compared with the foreground image*/
	 
	 if(background->cols < foreground->cols ||
	    background->rows < foreground->rows)
	 {
		 std::cout<< "please select background image with \
		 dimension greater or matching the foreground image"<<std::endl;
		 goto CLEANUP;
	 }
	 
	 /*Set the dimension of the composite image as the
	  * dimension of the foreground image */
	  OutputImage->rows = foreground->rows;
	  OutputImage->cols = foreground->cols;
	  OutputImage->mode = foreground->mode;
	  
	  total = OutputImage->rows*OutputImage->cols*RGBA_CHANNELS;
	  /*Allocate memory to store the pixelmap of the composite image*/
	  OutputImage->pixelmap = (unsigned char*)calloc(total,sizeof(unsigned char));
	  if(!OutputImage->pixelmap)
	  {
			std::cout<<"memory allocation failed"<<std::endl;
			goto CLEANUP;
	  }
	  
	  /*Allocate memory to store the pixelmap of the composite image*/
	  OutputImage->pixelmap_fliped = (unsigned char*)calloc(total,sizeof(unsigned char));
	  if(!OutputImage->pixelmap_fliped)
	  {
			std::cout<<"memory allocation failed"<<std::endl;
			goto CLEANUP;
	  }
	  
	  /*Get associated color of foreground*/
	  ret = Get_AssociatedColors(foreground);
	  if(ret != 0)
	  {
		  std::cout<<"Get_AssociatedColors failed for FG"<<std::endl;
		  goto CLEANUP;
	  }
	  
	  /*Get associated color of background*/
	  ret = Get_AssociatedColors(background);
	  if(ret != 0)
	  {
		  std::cout<<"Get_AssociatedColors failed for BG"<<std::endl;
		  goto CLEANUP;
	  }
	  
	  j = total;
	  /*Perform (A over B) operation*/
	  while(count < total)
	  {
		  alpha = (double)(foreground->assoc_color[count+3]/255.0);
		  //red
		  OutputImage->pixelmap[count] = (unsigned char)(foreground->assoc_color[count]+\
		  												(1-alpha)*background->assoc_color[count]+0.5);
		  //green												
		  OutputImage->pixelmap[count+1] = (unsigned char)(foreground->assoc_color[count+1]+\
		  													(1-alpha)*background->assoc_color[count+1]+0.5);
		  //blue
		  OutputImage->pixelmap[count+2] = (unsigned char)(foreground->assoc_color[count+2]+\
		  													(1-alpha)*background->assoc_color[count+2]+0.5);
		  //alpha													
		  OutputImage->pixelmap[count+3] = (unsigned char)(foreground->assoc_color[count+3]+\
															(1-alpha)*background->assoc_color[count+3]+0.5);
		  /*update the counter*/
		  count = count + 4;
		  j = j-4;  
	  }
	  
	  /*obtain the vertically flipped image for display*/
	  Image_Flip(OutputImage);
	  
	  /*update the global pointer*/
		ImageCount++;
		gImageInfo[ImageCount] = OutputImage;
	  
	  /*set the display to composite image*/
	  CurrentImageIndex = 2;
	  
	  return OutputImage;

	CLEANUP:
		free_imagememory(OutputImage);
		return NULL;
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
		
		if(image->pixelmap_fliped)
		{
			free(image->pixelmap_fliped);
			image->pixelmap_fliped = NULL;
		}
		
		if(image->assoc_color)
		{
			free(image->assoc_color);
			image->assoc_color = NULL;
		}
				
		free(image);
		image = NULL;
	}
}

/*Function to get associated colors of the image*/
int Get_AssociatedColors(imageinfo* image)
{
	int total = 0,count = 0;
	double alpha = 0;
	
	total = image->cols*image->rows*RGBA_CHANNELS;
	/*allocate memory to store the associated memory*/
	image->assoc_color = (unsigned char*)calloc(total,sizeof(unsigned char));
	if(!image->assoc_color)
	{
		std::cout<<"memory allocation failed"<<std::endl;	
		return -1;
	}
	
	while(count<total)
	{	
		alpha = (double)(image->pixelmap[count+3])/255.0;
		image->assoc_color[count] = (unsigned char) (alpha*image->pixelmap[count] + 0.5);
		image->assoc_color[count+1] = (unsigned char) (alpha*image->pixelmap[count+1] + 0.5);
		image->assoc_color[count+2] = (unsigned char) (alpha*image->pixelmap[count+2] + 0.5);
		image->assoc_color[count+3] = image->pixelmap[count+3];
		/*alpha remains unchanged*/
		count = count+4;
	}
	
	return 0;
}

/*Function to vertically flip the image*/
void Image_Flip(imageinfo* image)
{
	 pixel **pixmap = NULL;
	 pixel **pixmap_fliped = NULL;
	 int H=0,W=0;
	 
	 pixmap = new pixel*[image->rows];
	 pixmap[0] = new pixel[image->rows *image->cols];
	 /*update the pointers to point to each row of the image in continous memory allocation */
	 for (int i = 1; i < image->rows; i++)
		pixmap[i] = pixmap[i - 1] + image->cols;  
	
	 pixmap_fliped = new pixel*[image->rows];
	 pixmap_fliped[0] = new pixel[image->rows *image->cols];
	 /*update the pointers to point to each row of the image in continous memory allocation */
	 for (int i = 1; i < image->rows; i++)
		pixmap_fliped[i] = pixmap_fliped[i - 1] + image->cols;  
		
	 H = image->rows;
	 W = image->cols; 
	 
	 memcpy(pixmap[0],image->pixelmap,H*W*RGBA_CHANNELS);
	 
	 /*code snippet to invert the image upside down and store it in memory pointed by pixmap2[0]*/
	 for (int row = 0; row < H; row++)
	 {
		for (int col = 0; col < W; col++)
		{
			/*manipulate the pixel values to invert the image read from file*/
			pixmap_fliped[row][col].r = pixmap[H-1-row][col].r;
			pixmap_fliped[row][col].g = pixmap[H-1-row][col].g;
			pixmap_fliped[row][col].b = pixmap[H-1-row][col].b;
			pixmap_fliped[row][col].a = pixmap[H-1-row][col].a;
		}
	 }
	 
	  memcpy(image->pixelmap_fliped,pixmap_fliped[0],H*W*RGBA_CHANNELS);
	 
	 delete pixmap[0];
	 delete pixmap;
	 delete pixmap_fliped[0];
	 delete pixmap_fliped;
	
}
