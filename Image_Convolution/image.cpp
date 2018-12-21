/**************************************************************************
*  FILE NAME	: image.cpp
*
*  DESCRIPTION  : Utility functions used to perform convolution of 
*				  image and display the images.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  12th Oct,2018        Shashi Shivaraju        CPSC_6040_HW_04
*                       [C88650674]
***************************************************************************/
/*Header file  inclusion*/
#include "image.h"

/*Function to initialize glut*/
void InitGlut(int argc, char* argv[]) 
{
	/*initialise the GLUT utilities*/
	glutInit(&argc,argv);
    /*set window properties*/
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    /*create window*/
    glutCreateWindow("Convolution Viewer");
    /* set the window backgroung to black*/
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);  
	/* flush the OpenGL pipeline to the viewport */
	glFlush();
	/*set up the callback routines*/
	glutDisplayFunc(Manager::wrapperForDisplay);	  /* display callback */
    glutKeyboardFunc(Manager::handleKey);	  /* keyboard callback */
    glutReshapeFunc(Manager::wrapperForhandleReshape);   /* window resize callback */
}

  void Manager::wrapperForDisplay() 
 {
   Manager::getInstance()->drawImage();
 }
 
 void Manager::wrapperForhandleReshape(int w,int h) 
 {
   Manager::getInstance()->handleReshape(w,h);
 }

/*default constructor*/
Manager::Manager() 
{
	cols = WINDOW_WIDTH ;  /*width of image in pixels*/
	rows = WINDOW_HEIGHT;  /*height of image in pixels*/
	mode = ORIGINAL;
}

Manager::Manager(const Manager& in)
{
	inputfilename = in.inputfilename;
	outputfilename = in.outputfilename;
	filterfilename = in.filterfilename;
	cols = in.cols;  /*width of image in pixels*/
	rows = in.rows;  /*height of image in pixels*/
	mode = in.mode;
}

Manager::Manager(std::string infile,std::string outfile,std::string filterfile)
{
	inputfilename = infile;
	outputfilename = outfile;
	filterfilename = filterfile;
	cols = WINDOW_WIDTH ;  /*width of image in pixels*/
	rows = WINDOW_HEIGHT;  /*height of image in pixels*/
	mode = ORIGINAL;
}

Manager* Manager::getInstance() 
{
  if ( !instance ) instance = new Manager;
  return instance;
}

void Manager::setFilenames(std::string in,std::string out,std::string filter)
{
	inputfilename = in;
	outputfilename = out;
	filterfilename = filter;
}

/*Function to open image file and store it in a pixel map*/
int Manager::ReadImage(std::string filename)
{
	FILE *fp = NULL;
	unsigned char *tempImage = NULL; /*temporary pointer to store the pixel map*/
	int ret = 0; /*variable to check return value*/ 
	int count = 0,i=0,j=0; /*looping variables*/
	
	/*open the image file for reading*/
	ImageInput *image = ImageInput::open(filename);
	if(!image)
	{
		   std::cerr << "Could not open image for " << filename << ", error = " << geterror() << std::endl;
		   return -1;
	}
	
	/*get the image details*/
	const ImageSpec &spec = image->spec();
	
	cols = spec.width;
	rows = spec.height;
	
	
#ifdef DEBUG_MODE
	std::cout << filename <<std::endl;
	std::cout <<" with height = " << rows << " with width = "<< cols << " and channels = "<< spec.nchannels <<std::endl;
#endif /*DEBUG_MODE*/

	std::string file1("hawkori.ppm");

	/*allocate memory to store the pixels of the image*/
	tempImage = (unsigned char*)calloc(rows * cols * spec.nchannels,
													sizeof(unsigned char));
	if(!tempImage)
	{
		 std::cerr << "memory allocation failed "<< std::endl;
		 ret = -1;
		 goto CLEANUP;
	}
	
	/*store the image pixels to pixelmap*/
	image->read_image(TypeDesc::UINT8,tempImage);
	
	/*allocate memory to store the pixels of the image in RGBA format*/
	input_image = new pixel*[rows];
	input_image[0] = new pixel[rows * cols];
	 /*update the pointers to point to each row of the image in continous memory allocation */
	 for (int i = 1; i < rows; i++)
		input_image[i] = input_image[i - 1] + cols;
		
	/*allocate memory to store the pixels of the vertically flipped image in RGBA format*/
	input_image_flipped = new pixel*[rows];
	input_image_flipped[0] = new pixel[rows * cols];
	/*update the pointers to point to each row of the image in continous memory allocation */
	 for (int i = 1; i < rows; i++)
		input_image_flipped[i] = input_image_flipped[i - 1] + cols;
		
	/*allocate memory to store the pixels of the convoluted images in RGBA format*/
	convoluted_image = new pixel*[rows];
	convoluted_image[0] = new pixel[rows * cols];
	/*update the pointers to point to each row of the image in continous memory allocation */
	 for (int i = 1; i < rows; i++)
		convoluted_image[i] = convoluted_image[i - 1] + cols;
		
	/*allocate memory to store the pixels of the vertically flipped images in RGBA format*/
	convoluted_image_flipped = new pixel*[rows];
	convoluted_image_flipped[0] = new pixel[rows * cols];
	/*update the pointers to point to each row of the image in continous memory allocation */
	 for (int i = 1; i < rows; i++)
		convoluted_image_flipped[i] = convoluted_image_flipped[i - 1] + cols;
	
	/*image already in RGBA format*/
	if(RGBA_CHANNELS == spec.nchannels)
	{
		/*copy the pixels directly*/
		memcpy(input_image[0],tempImage,rows*cols*spec.nchannels);
	}
	else if(RGB_CHANNELS == spec.nchannels) /*image in RGB format*/
	{
		count = 0;
		for(i=0;i<rows;i++)
		{
			for(j=0;j<cols;j++)
			{
				input_image[i][j].r = tempImage[count]; /*R*/
				input_image[i][j].g = tempImage[count+1]; /*G*/
				input_image[i][j].b = tempImage[count+2]; /*B*/
				input_image[i][j].a = 255; /*A*/
				
				count = count + 3;
			}
		}
	}
	else if(MONO_CHANNELS == spec.nchannels)/*8 bit mono channel image*/
	{
		count = 0;
		for(i=0;i<rows;i++)
		{
			for(j=0;j<cols;j++)
			{
				input_image[i][j].r = tempImage[count]; /*R*/
				input_image[i][j].g = tempImage[count]; /*G*/
				input_image[i][j].b = tempImage[count]; /*B*/
				input_image[i][j].a = 255; /*A*/
				
				count = count + 1;
			}
		}
	}
	
	/*initialize the convolution image with original image*/
	//memcpy(convoluted_image[0],input_image[0],rows*cols*RGBA_CHANNELS);
	  memset(convoluted_image[0],0,rows*cols*RGBA_CHANNELS);
	  
	/*flip the image for display*/
	Image_Flip(input_image,input_image_flipped);
	
	mode = ORIGINAL;
	
	image->close();
    ImageInput::destroy(image);
	
	/*free the allocations*/	 
	if(tempImage)
	{
		free(tempImage);
		tempImage = NULL;
	}
	
	return 0;
				
	CLEANUP:
		 image->close();
		 ImageInput::destroy(image);
		 
		 if(tempImage)
		 {
			 free(tempImage);
			 tempImage = NULL;
		 }
		 
		 return ret;
}

/*
   Display Callback Routine:clear the screen and draw the pixelmap
   This routine is called every time the window on the screen needs
   to be redrawn, like if the window is iconized and then reopened
   by the user, and when the window is first created. It is also
   called whenever the program calls glutPostRedisplay()
*/
void Manager::drawImage()
{
	if(input_image)
	{
			/* set the window background to black*/
			glClearColor(0,0,0,0);
			glClear(GL_COLOR_BUFFER_BIT);
			glRasterPos2i(0,0);
			if(mode == ORIGINAL)
			{
				glDrawPixels(cols,rows,GL_RGBA,GL_UNSIGNED_BYTE,input_image_flipped[0]);
			}
			else if (mode == CONVOLUTED)
			{
				glDrawPixels(cols,rows,GL_RGBA,GL_UNSIGNED_BYTE,convoluted_image_flipped[0]);
			}
			glutReshapeWindow(cols,rows);
			// flush the OpenGL pipeline to the viewport
			glFlush();
	}
}

/*function to set the window size to fit the image size*/
void Manager::ReshapeWindow()
{
			glutReshapeWindow(cols,rows);
			// flush the OpenGL pipeline to the viewport
			glFlush();
	
}

/*
  Keyboard Callback Routine: This routine is called every time a key is pressed on the keyboard
*/
void Manager::handleKey(unsigned char key, int x, int y)
{
	int ret = 0,i = 0;

  switch(key){
	/*write the display to a file*/
	case 'w':
	case 'W':
	{
		Manager::getInstance()->WriteImage();
	}
	break;
	case 'c':
	case 'C':
	{
		Manager::getInstance()->ConvolveImage();
		
		/*display the convoluted image*/
		Manager::getInstance()->mode = CONVOLUTED;
		glutPostRedisplay();
		
	}
	break;
	case 'r':
	case 'R':
	{
		/*reset the convolution result*/
		Manager::getInstance()->ResetImage();
		
		/*display the original image*/
		Manager::getInstance()->mode = ORIGINAL;
		glutPostRedisplay();	
	}
	
	break;
	/*exit the program when user presses q,Q or esc*/  
    case 'q':		
    case 'Q':
    case 27:
    {
		/*need to deallocate resources before exiting*/
		Manager::getInstance()->freeallocations();
		exit(0);
	}
	break;
      
    default:
    		// not a valid key -- just ignore it
      return;
  }
}


/* 
	Reshape Callback Routine:
*/
void Manager::handleReshape(int w, int h)
{
	// set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// define the drawing coordinate system on the viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);// sets up a 2D orthographic viewing region
}


/*Function to vertically flip the image*/
void Manager::Image_Flip(pixel** original,pixel** flipped)
{		 
	 /*code snippet to invert the image upside down and store it in memory pointed by pixmap2[0]*/
	 for (int r = 0; r < rows; r++)
	 {
		for (int c = 0; c < cols; c++)
		{
			/*manipulate the pixel values to invert the image read from file*/
			flipped[r][c].r = original[rows-1-r][c].r;
			flipped[r][c].g = original[rows-1-r][c].g;
			flipped[r][c].b = original[rows-1-r][c].b;
			flipped[r][c].a = original[rows-1-r][c].a;
		}
	 }
}

void Manager::freeallocations()
{
	if(input_image)
	{
		if(input_image[0])
		{
			delete input_image[0];
		}
		delete input_image;
	}
	
	if(input_image_flipped)
	{
		if(input_image_flipped[0])
		{
			delete input_image_flipped[0];
		}
		delete input_image_flipped;
	}	
	
	if(convoluted_image)
	{
		if(convoluted_image[0])
		{
			delete convoluted_image[0];
		}
		delete convoluted_image;
	}	
	
	if(convoluted_image_flipped)
	{
		if(convoluted_image_flipped[0])
		{
			delete convoluted_image_flipped[0];
		}
		delete convoluted_image_flipped;
	}	
}

/*
    Routine to write the current framebuffer to an image file
*/
int Manager::WriteImage()
{
  	
  /*variable declarations*/
  int width = 0;
  int height = 0;
  unsigned char *pixmap = NULL;
  
  
  if(outputfilename.empty())
  {
	/* get a filename for the image */
	std::cout << "enter output image filename: ";
	std::cin >> outputfilename;
  }

  /* create the oiio file handler for the image*/
  ImageOutput *outfile = ImageOutput::create(outputfilename);
  if(!outfile){
    std::cerr << "Could not create output image for " << outputfilename << ", error = " << geterror() << std::endl;
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
  if(!outfile->open(outputfilename, spec)){
   std::cerr << "Could not open " << outputfilename << ", error = " << geterror() << std::endl;
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
    std::cerr << "Could not write image to " << outputfilename << ", error = " << geterror() << std::endl;
    ImageOutput::destroy(outfile);	
    return -1;
  }
  else
  {
    std::cout << "Image is stored as "<< outputfilename << std::endl;
  }
  
  /* close the image file after the image is written */
  if(!outfile->close())
  {
    std::cerr << "Could not close " << outputfilename << ", error = " << geterror() << std::endl;
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

/*Read the filter weights from file*/
int Manager::ReadFilter()
{
	int r=0,c=0;
	float weight = 0;
	float positive_weights = 0;
	float negative_weights = 0;
	
	
	/*open the file*/
	std::ifstream file(filterfilename);
	if(!file)
	{
		std::cout<<"unable to open the filter file" << filterfilename<<std::endl;
		return -1;
	}
	
	file >> kernel_size;
#ifdef  DEBUG_MODE
	std::cout << "Kernel size is "<<kernel_size<<std::endl;
	std::cout << "Kernel is:\n";
#endif /*DEBUG_MODE*/

	 /*store the kernel*/
	 kernel_weights = new float*[kernel_size];
	 float ** temp_weights = new float*[kernel_size];
	 
	 kernel_weights[0] = new float[kernel_size * kernel_size];
	 temp_weights[0] = new float[kernel_size * kernel_size];
	  for (int i = 1; i < kernel_size; i++)
	  {
			kernel_weights[i] = kernel_weights[i - 1] + kernel_size;
			temp_weights[i]   = temp_weights[i-1] + kernel_size;
	  }
	
	 
	 kernel_scalefactor = 0;
	 	
	 /*read the  weights of kernel and determine scale factor*/
	 for (r = 0; r < kernel_size; r++)
	 {
		for (c = 0; c < kernel_size; c++)
		{
			file >> kernel_weights[r][c];
			
			if(0 < kernel_weights[r][c])
			{
				positive_weights = positive_weights + kernel_weights[r][c];
			}
			else
			{
				negative_weights = positive_weights + kernel_weights[r][c];
			}
#ifdef  DEBUG_MODE
			std::cout<<kernel_weights[r][c];
#endif /*DEBUG_MODE*/
		}
#ifdef  DEBUG_MODE
		std::cout<<std::endl;
#endif /*DEBUG_MODE*/
	 }
	 
	 /*The scale factor to use is the maximum magnitude of either 
	  * (a) the sum of the positive weights or 
	  * (b) the sum of the negative weights.*/
	  kernel_scalefactor = (abs(positive_weights) > abs(negative_weights))?positive_weights:abs(negative_weights);
	  
	 
	 
	 
#ifdef  DEBUG_MODE
		std::cout << "kernel scale factor = "<<kernel_scalefactor<<std::endl;
		std::cout << "Horizontally flipped Kernel is:\n";
#endif /*DEBUG_MODE*/
	 
	
	 /*flip the kernel horizontally*/
	 for (r = 0; r < kernel_size; r++)
	 {
		for (c = 0; c < kernel_size; c++)
		{
			temp_weights[r][c] = kernel_weights[r][kernel_size-1-c];
#ifdef  DEBUG_MODE
			std::cout<<temp_weights[r][c];
#endif /*DEBUG_MODE*/
		}
#ifdef  DEBUG_MODE
		std::cout<<std::endl;
#endif /*DEBUG_MODE*/
	 }
	 
#ifdef  DEBUG_MODE
	    std::cout << "Vertically flipped Kernel is:\n";
#endif /*DEBUG_MODE*/
	 
	 /*flip the kernel vertically*/
	 for (r = 0; r < kernel_size; r++)
	 {
		for (c = 0; c < kernel_size; c++)
		{
			kernel_weights[r][c] = temp_weights[kernel_size-1-r][c];
#ifdef  DEBUG_MODE
			std::cout<<kernel_weights[r][c];
#endif /*DEBUG_MODE*/
		}
#ifdef  DEBUG_MODE
		std::cout<<std::endl;
#endif /*DEBUG_MODE*/
	 }

	/*deallocate the memory*/
	delete temp_weights[0];
	delete temp_weights;
	/*close the file*/
	file.close();
	return 0;
}

/*reset the convolution result*/
void Manager::ResetImage()
{
	if(convoluted_image)
	{
		memcpy(convoluted_image[0],input_image[0],rows*cols*RGBA_CHANNELS);
		memset(convoluted_image[0],0,rows*cols*RGBA_CHANNELS);
	}
	
}

void Manager::ConvolveImage()
{
	pixel** input = NULL;
	pixelfloat **inter_image = NULL;
	float pixelred = 0,pixelgreen = 0,pixelblue = 0;
	float sumred = 0,sumgreen = 0,sumblue = 0;
	float redmin = 0,redmax = 0,greenmin = 0,greenmax = 0,bluemin = 0,bluemax = 0;
	float redrange = 0,greenrange = 0,bluerange = 0;
	int border = kernel_size/2;
	int R = 0,C = 0,r=0,c=0; 
	int count = 0;
	
	if(mode == ORIGINAL)
	{
		input = input_image;
	}
	else
	{
		input = convoluted_image;
	}	
	
	/*allocate memory to store intermediate float image*/
	inter_image = new pixelfloat*[rows];
	inter_image[0] = new pixelfloat[rows * cols];
	/*update the pointers to point to each row of the image in continous memory allocation */
	for (int i = 1; i < rows; i++)
		inter_image[i] = inter_image[i - 1] + cols;
	
	/*perform convolution of the image with kernel by excluding the borders*/
	for(R = 0;R < rows;R++)
	{
		for(C=0;C < cols;C++)
		{
			sumred = 0;
			sumgreen = 0;
			sumblue = 0;
			for(r=-border;r<=border;r++)
			{
				for(c=-border;c<=border;c++)
				{
					/*check if pixel index is outside the image*/
					if(R+r < 0 || R+r >= rows || C+c < 0 || C+c >= cols )
					{
						continue;
					}
					sumred = sumred + input[R+r][C+c].r*kernel_weights[r+border][c+border];
					//std::cout <<"pixel = "<<input[R+r][C+c].r<< \
					"weight = "<<kernel_weights[r+border][c+border]<<std::endl;
					sumgreen = sumgreen + input[R+r][C+c].g*kernel_weights[r+border][c+border];
					sumblue = sumblue + input[R+r][C+c].b*kernel_weights[r+border][c+border];
				}
			}
			
			inter_image[R][C].r = sumred/(float)kernel_scalefactor;
			inter_image[R][C].g = sumgreen/(float)kernel_scalefactor;
			inter_image[R][C].b = sumblue/(float)kernel_scalefactor;
			
			
			if(!count|| inter_image[R][C].r < redmin)
				redmin = inter_image[R][C].r;

			if(!count|| inter_image[R][C].r > redmax)
				redmax = inter_image[R][C].r;
				
			if(!count|| inter_image[R][C].g < greenmin)
				greenmin = inter_image[R][C].g;

			if(!count|| inter_image[R][C].g > greenmax)
				greenmax = inter_image[R][C].g;
				
			if(!count|| inter_image[R][C].b < bluemin)
				bluemin = inter_image[R][C].b;

			if(!count|| inter_image[R][C].b > bluemax)
				bluemax = inter_image[R][C].b;
				
			count ++;
		}
	}
	
	/*Normalize the results*/
	redrange = redmax-redmin;
	greenrange = greenmax-greenmin;
	bluerange = bluemax-bluemin;
	
	for(R = 0;R < rows;R++)
	{
		for(C = 0;C < cols;C++)
		{
			pixelred = inter_image[R][C].r;
			pixelgreen = inter_image[R][C].g;
			pixelblue = inter_image[R][C].b;
			
			pixelred = ((pixelred-redmin)/redrange)*255;
			pixelgreen = ((pixelgreen-greenmin)/greenrange)*255;
			pixelblue = ((pixelblue-bluemin)/bluerange)*255;
			
			/*rounding the values to the next highest integer if value above 0.5  */
			if(pixelred-(unsigned char)pixelred > 0.5)
				convoluted_image[R][C].r = (unsigned char)(pixelred+1);
			else
				convoluted_image[R][C].r = (unsigned char)pixelred;
				
			if(pixelgreen-(unsigned char)pixelgreen > 0.5)
				convoluted_image[R][C].g = (unsigned char)(pixelgreen+1);
			else
				convoluted_image[R][C].g = (unsigned char)pixelgreen;
				
			if(pixelblue-(unsigned char)pixelblue > 0.5)
				convoluted_image[R][C].b = (unsigned char)(pixelblue+1);
			else
				convoluted_image[R][C].b = (unsigned char)pixelblue;
			
				convoluted_image[R][C].a = 255;
		}
	}
	
	/*Flip the convolution result for display*/
	Image_Flip(convoluted_image,convoluted_image_flipped);
}

void Manager::buildGaborKernel()
{
	int R=0,C=0;
	int x=0,y=0;
	float positive_weights = 0;
	float negative_weights = 0;
	
	kernel_scalefactor = 0;
	kernel_size = 4*sigma+1; 
	
	//std::cout <<"Gabor "<<theta <<" "<< sigma<<" "<< period<<std::endl;
	  
	/*allocate memory to store the kernel*/
	kernel_weights = new float*[kernel_size];
    float ** temp_weights = new float*[kernel_size];
	 
	kernel_weights[0] = new float[kernel_size * kernel_size];
	temp_weights[0] = new float[kernel_size * kernel_size];
	for (int i = 1; i < kernel_size; i++)
	{
		kernel_weights[i] = kernel_weights[i - 1] + kernel_size;
		temp_weights[i]   = temp_weights[i-1] + kernel_size;
	}
	
	/*initial (x,y) position*/
	x = kernel_size/2;
	y = -kernel_size/2;
	
	for(R=0;R<kernel_size;R++)
	{
		for(C=0;C<kernel_size;C++)
		{		   
			kernel_weights[R][C] = getGaborWeight(x,y);
			if(0 < kernel_weights[R][C])
			{
				positive_weights = positive_weights + kernel_weights[R][C];
			}
			else
			{
				negative_weights = positive_weights + kernel_weights[R][C];
			}
#ifdef DEBUG_MODE
			std::cout<< kernel_weights[R][C]<<" ";
#endif /*DEBUG_MODE*/
			y = y+1;  
		}
		x = x-1;
		y = -kernel_size/2;
#ifdef DEBUG_MODE
		std::cout<<std::endl;
#endif /*DEBUG_MODE*/
	}
	
	 /*The scale factor to use is the maximum magnitude of either 
	  * (a) the sum of the positive weights or 
	  * (b) the sum of the negative weights.*/
	  kernel_scalefactor = (abs(positive_weights) > abs(negative_weights))?positive_weights:abs(negative_weights);
	
	 /*since its a symmetric matrix we do not need to flip it*/
		
}

void Manager::setGaborSpecs(float the,float sig,float T)
{
	theta =  (the*M_PI/180.0); /*convert to radians*/
	sigma = sig;
	period = T; 
}

float Manager::getGaborWeight(int x,int y)
{
	float kweight = 0;
	float x_cap = 0;
	float y_cap = 0;
	
	x_cap = x*cos(theta)+y*sin(theta);
	y_cap = -x*sin(theta)+y*cos(theta);
	
	kweight = exp(-((x_cap*x_cap)+(y_cap*y_cap))/(2*(sigma*sigma)))*cos((2*M_PI*x_cap)/period);
	
	return kweight;
}
