/**********************************************************************
*  FILE NAME	: imgview_func.cpp
*
*  DESCRIPTION  : Utility functions used for display of image using 
*				  OpenGL and GLUT
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  5th Sep,2018         Shashi Shivaraju        CPSC_6040_HW_01
*                       [C88650674]
***********************************************************************/
#include "imageview.h"
using namespace std;

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
		if(gImageInfo[CurrentImageIndex]->pixelmap)
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
							GL_RGBA,GL_UNSIGNED_BYTE,gImageInfo[CurrentImageIndex]->pixelmap);
			}
			else if(R_CHANNEL == gImageInfo[CurrentImageIndex]->mode &&
									gImageInfo[CurrentImageIndex]->rcolormap)
			{
				glDrawPixels(gImageInfo[CurrentImageIndex]->cols,
							gImageInfo[CurrentImageIndex]->rows,
							GL_RED,GL_UNSIGNED_BYTE,gImageInfo[CurrentImageIndex]->rcolormap);
		
			}
			else if(G_CHANNEL == gImageInfo[CurrentImageIndex]->mode &&
									gImageInfo[CurrentImageIndex]->gcolormap)
			{	
				glDrawPixels(gImageInfo[CurrentImageIndex]->cols,
							gImageInfo[CurrentImageIndex]->rows,
							GL_GREEN,GL_UNSIGNED_BYTE,gImageInfo[CurrentImageIndex]->gcolormap);
		
			}
			else if(B_CHANNEL == gImageInfo[CurrentImageIndex]->mode &&
									gImageInfo[CurrentImageIndex]->bcolormap)
			{
				glDrawPixels(gImageInfo[CurrentImageIndex]->cols,
							gImageInfo[CurrentImageIndex]->rows,
							GL_BLUE,GL_UNSIGNED_BYTE,gImageInfo[CurrentImageIndex]->bcolormap);
		
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


/*Special event Callback Routine*/
void handleSpecialKey(int key,int x,int y)
{
	switch(key)
	{
		/*browse the images using left and right arrows*/
		case GLUT_KEY_LEFT:
		{
			/*display previous image loaded before current image*/
			if(0 < CurrentImageIndex)
			{
				CurrentImageIndex--;
				/*reshape the window for the current image*/
				ReshapeWindow();
				/*render the image in the window*/
				glutPostRedisplay();
			}
			else
			{
				CurrentImageIndex = ImageCount;
				/*reshape the window for the current image*/
				ReshapeWindow();
				/*render the image in the window*/
				glutPostRedisplay();
			}
		}
		break;
		case GLUT_KEY_RIGHT:
		{
			/*display next image loaded after current image*/
			if(CurrentImageIndex < ImageCount)
			{
				CurrentImageIndex++;
				/*reshape the window for the current image*/
				ReshapeWindow();
				/*render the image in the window*/
				glutPostRedisplay();
			}	
			else
			{
				CurrentImageIndex = 0;
				/*reshape the window for the current image*/
				ReshapeWindow();
				/*render the image in the window*/
				glutPostRedisplay();
				
			}
		}
		break;
		default:
		{
			return;
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
	/*read the file and display it*/
	case 'r':
	case 'R':
	{
		char inputfilename[MAX_FILE_NAME];
		
		if((ImageCount+1) == MAX_IMAGES_SUPPORTED)
		{
			std::cout<< "Image load limit reached;restart to load new images"<<std::endl;
			return;
		}
		
		/*request for filename from the user*/
		std::cout<< "please enter the image filename : "<< std::endl;
		std::cin>>inputfilename;
		
		if(!ReadImage(inputfilename))
		{
			std::cerr<< "ReadImage() function failed for "<< inputfilename<<std::endl;
		}
		else
		{
				CurrentImageIndex = ImageCount;
				/*reshape the window for the current image*/
				ReshapeWindow();
				/*render the image in the window*/
				glutPostRedisplay();
		}
	}	
	break;
	/*write the display to a file*/
	case 'w':
	case 'W':
	{
		ret = WriteImage();
		if(0 != ret)
		{
			std::cerr<< "WriteImage() function failed " <<std::endl;
		}
	}
	break;
	/*invert the colours of display*/
	case 'i':
	case 'I':
	{
		invertColors();
	}
	break;
	/*display only RED channel greyscale*/
	case '1':
	{
		if(gImageInfo[CurrentImageIndex])
		{
			gImageInfo[CurrentImageIndex]->mode = R_CHANNEL;
			UpdateChannelPixelmap();
		}
	}
	break;
	/*display only GREEN channel greyscale*/
	case '2':
	{
		if(gImageInfo[CurrentImageIndex])
		{
			gImageInfo[CurrentImageIndex]->mode = G_CHANNEL;
			UpdateChannelPixelmap();
		}
	}
	break;
	/*display only BLUE channel greyscale*/
	case '3':
	{
		if(gImageInfo[CurrentImageIndex])
		{
			gImageInfo[CurrentImageIndex]->mode = B_CHANNEL;
			UpdateChannelPixelmap();
		}
	}
	break;
	/*display RGBA image*/
	case 'o':
	case 'O':
	{
		if(gImageInfo[CurrentImageIndex])
		{
			gImageInfo[CurrentImageIndex]->mode = RGBA;
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
				if(gImageInfo[i]->pixelmap)
				{
					free(gImageInfo[i]->pixelmap);
					gImageInfo[i]->pixelmap = NULL;
				}
				if(gImageInfo[i]->rcolormap)
				{
					free(gImageInfo[i]->rcolormap);
					gImageInfo[i]->rcolormap = NULL;
				}
				if(gImageInfo[i]->gcolormap)
				{
					free(gImageInfo[i]->gcolormap);
					gImageInfo[i]->gcolormap = NULL;
				}
				if(gImageInfo[i]->bcolormap)
				{
					free(gImageInfo[i]->bcolormap);
					gImageInfo[i]->bcolormap = NULL;
				}
				free(gImageInfo[i]);
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
	image->read_image(TypeDesc::UINT8,tempImage + (ImageConfig->rows -1)*scanlinesize, // offset to last
						AutoStride, /*default x stride*/
						-scanlinesize, /*special y stride*/
						AutoStride); /*default z stride)*/
	
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
	ImageConfig->rcolormap = NULL;
	ImageConfig->gcolormap = NULL;
	ImageConfig->bcolormap = NULL;
		
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
int WriteImage(){
  	
  /*variable declarations*/
  int width = 0;
  int height = 0;
  unsigned char *pixmap = NULL;
  string outfilename;

  /* get a filename for the image */
  cout << "enter output image filename: ";
  cin >> outfilename;

  /* create the oiio file handler for the image*/
  ImageOutput *outfile = ImageOutput::create(outfilename);
  if(!outfile){
    cerr << "Could not create output image for " << outfilename << ", error = " << geterror() << endl;
    return -1;
  }
  
  /*Get the window size*/
  width  = glutGet(GLUT_WINDOW_WIDTH);
  height = glutGet(GLUT_WINDOW_HEIGHT);
  
  /*Allocate memeory for the pixelmap*/
  pixmap = (unsigned char*)calloc(width*height*RGBA_CHANNELS,sizeof(unsigned char));
  if(!pixmap)
  {
	  std::cerr<<"memory allocation failed"<<std::endl;
	  return -1;
  }
  
  /* get the current pixels from the OpenGL framebuffer and store in pixmap */
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixmap);

  /*The header will indicate an image is of size
   *  width x height and with 4 channels per pixel (RGBA).
   *  All channels will be of type unsigned char*/
  ImageSpec spec(width, height, RGBA_CHANNELS, TypeDesc::UINT8);
  
  
  /* Open the file for writing the image */ 
  if(!outfile->open(outfilename, spec)){
    cerr << "Could not open " << outfilename << ", error = " << geterror() << endl;
	    ImageOutput::destroy(outfile);
    return -1;
  }

   int scanlinesize = width * RGBA_CHANNELS * sizeof(unsigned char);

  /* write the image to the file */
  /*Flip an image vertically upon writing, by using negative y stride,
   * since the pixelmap has data starting from lower left cormer of window */
  if(!outfile->write_image(TypeDesc::UINT8,
							pixmap + (height-1)*scanlinesize, /* offset to last */
							AutoStride, /*default x stride*/
							-scanlinesize, /*special y stride*/
							AutoStride)) /*default z stride*/
  {
    cerr << "Could not write image to " << outfilename << ", error = " << geterror() << endl;
	   ImageOutput::destroy(outfile);	
    return -1;
  }
  else
  {
    cout << "Image is stored as "<< outfilename << endl;
  }
  
  /* close the image file after the image is written */
  if(!outfile->close())
  {
    cerr << "Could not close " << outfilename << ", error = " << geterror() << endl;
	    ImageOutput::destroy(outfile);
    return -1;
  }

  if(pixmap)
  {
	  free(pixmap);
	  pixmap = NULL;
  }
  
  return 0;
}
/*Function to invert the colors in the pixelmap*/
void invertColors()
{
	int i = 0,totalsize = 0;
	
	/*check if the image is loaded*/
	if(!gImageInfo[CurrentImageIndex])
	{
		std::cerr<< "No image loaded to invert colors" <<std::endl;
		return;
	}
	
	totalsize = gImageInfo[CurrentImageIndex]->cols*gImageInfo[CurrentImageIndex]->rows;
	
	if(RGBA == gImageInfo[CurrentImageIndex]->mode)
	{
		totalsize = gImageInfo[CurrentImageIndex]->cols*gImageInfo[CurrentImageIndex]->rows*RGBA_CHANNELS;
		/*invert the colors */
		for(i=0;i<totalsize;i++)
		{
			gImageInfo[CurrentImageIndex]->pixelmap[i] = 255 - gImageInfo[CurrentImageIndex]->pixelmap[i];		
		}
	}
	else if(R_CHANNEL == gImageInfo[CurrentImageIndex]->mode &&
			gImageInfo[CurrentImageIndex]->rcolormap)
	{
		/*invert the colors of Red Channel*/
		for(i=0;i<totalsize;i++)
		{
			gImageInfo[CurrentImageIndex]->rcolormap[i] = 255 - gImageInfo[CurrentImageIndex]->rcolormap[i];		
		}
	}
	else if(G_CHANNEL == gImageInfo[CurrentImageIndex]->mode &&
			gImageInfo[CurrentImageIndex]->gcolormap)
	{
		/*invert the colors of Green Channel*/
		for(i=0;i<totalsize;i++)
		{
			gImageInfo[CurrentImageIndex]->gcolormap[i] = 255 - gImageInfo[CurrentImageIndex]->gcolormap[i];		
		}	
	}
	else if(B_CHANNEL == gImageInfo[CurrentImageIndex]->mode &&
			gImageInfo[CurrentImageIndex]->bcolormap)
	{
		/*invert the colors of Blue Channel*/
		for(i=0;i<totalsize;i++)
		{
			gImageInfo[CurrentImageIndex]->bcolormap[i] = 255 - gImageInfo[CurrentImageIndex]->bcolormap[i];		
		}
	}
		
	/*render the image in the window*/
	glutPostRedisplay();
	
}

/*Function to determine the channel pixels from RGBA pixelmap*/
void UpdateChannelPixelmap()
{
	int i = 0,j = 0;
	int totalsize = 0;
	
	/*check if the image is loaded*/
	if(!gImageInfo[CurrentImageIndex])
	{
		std::cerr<< "No image loaded to invert colors" <<std::endl;
		return;
	}
	
	/*check if individual channel pixelmaps are already present */
	if(gImageInfo[CurrentImageIndex]->rcolormap &&
		gImageInfo[CurrentImageIndex]->gcolormap &&
		gImageInfo[CurrentImageIndex]->bcolormap)
	{
		return;
	}	
	
	/*number of bytes for each channel */
	totalsize = gImageInfo[CurrentImageIndex]->cols*gImageInfo[CurrentImageIndex]->rows;
	
	/*allocate memory for RED channel*/
	gImageInfo[CurrentImageIndex]->rcolormap = (unsigned char*)calloc(totalsize,sizeof(unsigned char));
	if(!gImageInfo[CurrentImageIndex]->rcolormap)
	{
		 std::cerr << "memory allocation failed "<< std::endl;
		return;
	}
	
	/*allocate memory for GREEN channel*/
	gImageInfo[CurrentImageIndex]->gcolormap = (unsigned char*)calloc(totalsize,sizeof(unsigned char));
	if(!gImageInfo[CurrentImageIndex]->gcolormap)
	{
		 std::cerr << "memory allocation failed "<< std::endl;
		return;
	}
	
	/*allocate memory for BLUE channel*/
	gImageInfo[CurrentImageIndex]->bcolormap = (unsigned char*)calloc(totalsize,sizeof(unsigned char));
	if(!gImageInfo[CurrentImageIndex]->bcolormap)
	{
		 std::cerr << "memory allocation failed "<< std::endl;
		return;
	}
	
	/*update the channel pixel maps*/
		i = 0;
		j = 0; 
		totalsize = gImageInfo[CurrentImageIndex]->cols*gImageInfo[CurrentImageIndex]->rows*RGBA_CHANNELS;
		
		while(i<totalsize)
		{
			gImageInfo[CurrentImageIndex]->rcolormap[j] = gImageInfo[CurrentImageIndex]->pixelmap[i];		
			gImageInfo[CurrentImageIndex]->gcolormap[j] = gImageInfo[CurrentImageIndex]->pixelmap[i+1];
			gImageInfo[CurrentImageIndex]->bcolormap[j] = gImageInfo[CurrentImageIndex]->pixelmap[i+2];
			
			j++;
			i = i+4;
		}
}


