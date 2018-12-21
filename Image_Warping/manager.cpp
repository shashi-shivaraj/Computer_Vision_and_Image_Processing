/**************************************************************************
*  FILE NAME	: manager.cpp
*
*  DESCRIPTION  : Utility functions used to perform warping of 
*				  image and to display the image.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  27th Oct,2018        Shashi Shivaraju        CPSC_6040_HW_05
*                       [C88650674]
***************************************************************************/
/*Header file  inclusion*/
#include "manager.h"

/*Function to initialize glut*/
void InitGlut(int argc, char* argv[]) 
{
	/*initialise the GLUT utilities*/
	glutInit(&argc,argv);
    /*set window properties*/
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    /*create window*/
    glutCreateWindow("Warp Viewer");
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
}

Manager::Manager(const Manager& in)
{
	inputfilename = in.inputfilename;
	outputfilename = in.outputfilename;
	cols = in.cols;  /*width of image in pixels*/
	rows = in.rows;  /*height of image in pixels*/
}

Manager::Manager(std::string infile,std::string outfile)
{
	inputfilename = infile;
	outputfilename = outfile;
	cols = WINDOW_WIDTH ;  /*width of image in pixels*/
	rows = WINDOW_HEIGHT;  /*height of image in pixels*/

}

Manager* Manager::getInstance() 
{
  if ( !instance ) instance = new Manager;
  return instance;
}

void Manager::setFilenames(std::string in,std::string out)
{
	inputfilename = in;
	outputfilename = out;
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
	
	int scanlinesize = spec.width * spec.nchannels * sizeof(unsigned char);
	
	
#ifdef DEBUG_MODE
	std::cout << filename <<std::endl;
	std::cout <<" with height = " << rows << " with width = "<< cols << " and channels = "<< spec.nchannels <<std::endl;
#endif /*DEBUG_MODE*/


	/*allocate memory to store the pixels of the image*/
	tempImage = (unsigned char*)calloc(rows * cols * spec.nchannels,
													sizeof(unsigned char));
	if(!tempImage)
	{
		 std::cerr << "memory allocation failed "<< std::endl;
		 ret = -1;
		 goto CLEANUP;
	}
	
	/*Flip an image vertically upon reading, by using negative y stride.
	 * since OpenGL render from bottom left corner of raster*/
	image->read_image(TypeDesc::UINT8,tempImage + (rows -1)*scanlinesize, // offset to last
						AutoStride, /*default x stride*/
						-scanlinesize, /*special y stride*/
						AutoStride); /*default z stride)*/
	
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
	  
	/*flip the image for display*/
	Image_Flip(input_image,input_image_flipped,rows,cols);
	
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
	if(warped_image)
	{
			/* set the window background to black*/
			glClearColor(0,0,0,0);
			glClear(GL_COLOR_BUFFER_BIT);
			glRasterPos2i(0,0);
			glDrawPixels(warped_cols,warped_rows,GL_RGBA,GL_UNSIGNED_BYTE,warped_image[0]);
			glutSwapBuffers();
			glutReshapeWindow(warped_cols,warped_rows);
			// flush the OpenGL pipeline to the viewport
			glFlush();
	}
}

/*function to set the window size to fit the image size*/
void Manager::ReshapeWindow()
{
			glutReshapeWindow(warped_cols,warped_rows);
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
void Manager::Image_Flip(pixel** original,pixel** flipped,int R,int C)
{		 
	 /*code snippet to invert the image upside down and store it in memory pointed by pixmap2[0]*/
	 for (int r = 0; r < R; r++)
	 {
		for (int c = 0; c < C; c++)
		{
			/*manipulate the pixel values to invert the image read from file*/
			flipped[r][c].r = original[R-1-r][c].r;
			flipped[r][c].g = original[R-1-r][c].g;
			flipped[r][c].b = original[R-1-r][c].b;
			flipped[r][c].a = original[R-1-r][c].a;
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
	
	
	if(warped_image)
	{
		if(warped_image[0])
		{
			delete warped_image[0];
		}
		delete warped_image;
	}
	
	if(warped_image_flipped)
	{
		if(warped_image_flipped[0])
		{
			delete warped_image_flipped[0];
		}
		delete warped_image_flipped;
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
  width  = warped_cols;
  height = warped_rows;
  
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
							warped_image_flipped[0])) /*default z stride*/
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
  
  return 0;
}

void Manager::getInputImageSize(int &c,int &r)
{
	c = cols;
	r = rows;
	
}

void Manager::ForwardTransform(Matrix3D &M)
{
	Vector3D Corner1;
	Vector3D Corner2;
	Vector3D Corner3;
	Vector3D Corner4;
	
	
	double minX = 0,minY = 0,maxX= 0,maxY = 0;
	
	/*corner = (0,0)*/
	Corner1 = M * Corner1;
	Corner1.x = Corner1.x/Corner1.z;
	Corner1.y = Corner1.y/Corner1.z;
	Corner1.z = Corner1.z/Corner1.z;
	
	minX = Corner1.x;
	maxX = Corner1.x;
	minY = Corner1.y;
	maxY = Corner1.y;
	
	/*corner = (0,H)*/
	Corner2.y = rows;
	Corner2 = M * Corner2;
	Corner2.x = Corner2.x/Corner2.z;
	Corner2.y = Corner2.y/Corner2.z;
	Corner2.z = Corner2.z/Corner2.z;
	
	if(minX > Corner2.x)
		minX = Corner2.x;
	if(maxX < Corner2.x)
		maxX = Corner2.x;
	if(minY > Corner2.y)
		minY = Corner2.y;
	if(maxY < Corner2.y)
		maxY = Corner2.y;
		
	/*corner = (W,0)*/
	Corner3.x = cols;
	Corner3 = M * Corner3;
	Corner3.x = Corner3.x/Corner3.z;
	Corner3.y = Corner3.y/Corner3.z;
	Corner3.z = Corner3.z/Corner3.z;
	 
	if(minX > Corner3.x)
		minX = Corner3.x;
	if(maxX < Corner3.x)
		maxX = Corner3.x;
	if(minY > Corner3.y)
		minY = Corner3.y;
	if(maxY < Corner3.y)
		maxY = Corner3.y;
	
	
	/*corner = (W,H)*/
	Corner4.x = cols;
	Corner4.y = rows;
	Corner4 = M * Corner4;
	Corner4.x = Corner4.x/Corner4.z;
	Corner4.y = Corner4.y/Corner4.z;
	Corner4.z = Corner4.z/Corner4.z;
	
	if(minX > Corner4.x)
		minX = Corner4.x;
	if(maxX < Corner4.x)
		maxX = Corner4.x;
	if(minY > Corner4.y)
		minY = Corner4.y;
	if(maxY < Corner4.y)
		maxY = Corner4.y;
	
	
	warped_cols = int (maxX - minX + 0.5);
	warped_rows = int (maxY - minY + 0.5);
	
#ifdef DEBUG_MODE
	std::cout<<"output corners are :"<<"\n"<<Corner1.x << " "<<Corner1.y <<std::endl;
	std::cout<< Corner2.x << " "<<Corner2.y <<std::endl;
	std::cout<< Corner3.x << " "<<Corner3.y <<std::endl;
	std::cout<< Corner4.x << " "<<Corner4.y <<std::endl;
	std::cout<< "max X = "<<maxX << " max Y = "<<maxY<<std::endl;
	std::cout<< "min X = "<<minX << " min Y = "<<minY<<std::endl;
	std::cout<< "Output image size  = "<<warped_cols <<" X " << warped_rows <<std::endl; 
	std::cout<< "Origin is translated by (" <<-minX << "," << -minY << ")"<<std::endl;  
#endif /*DEBUG_MODE*/

	/*add the translation to the forward map*/
	Matrix3D T;
	T[0][2] = -minX;
	T[1][2] = -minY;
	M =  T*M;
	
	/*find the new corners of the output image*/
	Vector3D NCorner1;
	Vector3D NCorner2;
	Vector3D NCorner3;
	Vector3D NCorner4;

	/*corner = (0,0)*/
	NCorner1 = M * NCorner1;
	warped_xycorners[0].x = NCorner1.x/NCorner1.z;
	warped_xycorners[0].y = NCorner1.y/NCorner1.z;
	
	/*corner = (0,H)*/
	NCorner2.y = rows;
	NCorner2 = M * NCorner2;
	warped_xycorners[1].x = NCorner2.x/NCorner2.z;
	warped_xycorners[1].y = NCorner2.y/NCorner2.z;
	
	/*corner = (W,0)*/
	NCorner3.x = cols;
	NCorner3 = M * NCorner3;
	warped_xycorners[3].x = NCorner3.x/NCorner3.z;
	warped_xycorners[3].y = NCorner3.y/NCorner3.z;
	
	
	/*corner = (W,H)*/
	NCorner4.x = cols;
	NCorner4.y = rows;
	NCorner4 = M * NCorner4;
	warped_xycorners[2].x = NCorner4.x/NCorner4.z;
	warped_xycorners[2].y = NCorner4.y/NCorner4.z;

#ifdef DEBUG_MODE
	std::cout<<"output corners are :"<<"\n"<<NCorner1.x << " "<<NCorner1.y << " " <<NCorner1.z <<std::endl;
	std::cout<< NCorner2.x << " "<<NCorner2.y << " " <<NCorner2.z<<std::endl;
	std::cout<< NCorner3.x << " "<<NCorner3.y  << " " <<NCorner3.z<<std::endl;
	std::cout<< NCorner4.x << " "<<NCorner4.y  << " " <<NCorner4.z<<std::endl;  
#endif /*DEBUG_MODE*/

	
	/*allocate memory to store the pixels of the image in RGBA format*/
	warped_image = new pixel*[warped_rows];
	warped_image[0] = new pixel[warped_rows * warped_cols];
	 /*update the pointers to point to each row of the image in continous memory allocation */
	 for (int i = 1; i < warped_rows; i++)
		warped_image[i] = warped_image[i - 1] + warped_cols;
		
	/*allocate memory to store the pixels of the vertically flipped image in RGBA format*/
	warped_image_flipped = new pixel*[warped_rows];
	warped_image_flipped[0] = new pixel[warped_rows * warped_cols];
	/*update the pointers to point to each row of the image in continous memory allocation */
	 for (int i = 1; i < warped_rows; i++)
		warped_image_flipped[i] = warped_image_flipped[i - 1] + warped_cols;
	
	return;
}

void Manager::InverseTransform(Matrix3D invM)
{
	int i = 0,j = 0;
	int r = 0,c = 0;
	Vector3D inputPixelLocation;
	Vector3D outputPixelLocation;
	
	/*map each pixel of input image to the output warped image*/
	for(i=0;i<warped_rows;i++)
	{
		for(j=0;j<warped_cols;j++)
		{
			
			outputPixelLocation.x = j;
			outputPixelLocation.y = i;
			outputPixelLocation.z = 1;
			
			inputPixelLocation = invM * outputPixelLocation;
			inputPixelLocation.x = inputPixelLocation.x/inputPixelLocation.z;
			inputPixelLocation.y = inputPixelLocation.y/inputPixelLocation.z;
			inputPixelLocation.z = inputPixelLocation.z/inputPixelLocation.z;
			
			r = int(inputPixelLocation.y + 0.5);
			c = int(inputPixelLocation.x + 0.5);
			
			
			//std::cout << "Output pixel (R,C) ("<<i <<","<<j<<")"<<std::endl;
			//std::cout << "Input  pixel (R,C) ("<<r <<","<<c<<")"<<std::endl;
			
			if(r > 0 && r <= rows && c > 0 && c < cols)
			{
				/*handle boundary case*/
				if(c == cols)
					c = cols - 1;
		
				if(r == rows)
					r = rows - 1;
				
				warped_image[i][j].r = input_image[r][c].r; /*R*/
				warped_image[i][j].g = input_image[r][c].g; /*G*/
				warped_image[i][j].b = input_image[r][c].b; /*B*/
				warped_image[i][j].a = input_image[r][c].a; /*A*/
			}
			else
			{
				warped_image[i][j].r = 255; /*R*/
				warped_image[i][j].g = 255; /*G*/
				warped_image[i][j].b = 255; /*B*/
				warped_image[i][j].a = 255; /*A*/
			}
		}
	}
	
	/*flip the image for display*/
	Image_Flip(warped_image,warped_image_flipped,warped_rows,warped_cols);
	
}


/*Function to perform bilinear warp*/
void Manager::BilinearWarp()
{
	int r = 0,c = 0;
	Vector2D XYVec, UVVec;
	BilinearCoeffs coeff;
	
	setbilinear(warped_cols, warped_rows, warped_xycorners, coeff);
	
	for (int i = 0; i < warped_rows; i++) 
	{
		for (int j = 0; j < warped_cols; j++) 
		{
			XYVec.x = j;
			XYVec.y = i;
			
			invbilinear(coeff, XYVec, UVVec);
			
			r = (int)(UVVec.y+0.5);
			c = (int)(UVVec.x+0.5);

			if(r > 0 && r <= rows && c > 0 && c < cols)
			{
				/*handle boundary case*/
				if(c == cols)
					c = cols - 1;
		
				if(r == rows)
					r = rows - 1;
				
				warped_image[i][j].r = input_image[r][c].r; /*R*/
				warped_image[i][j].g = input_image[r][c].g; /*G*/
				warped_image[i][j].b = input_image[r][c].b; /*B*/
				warped_image[i][j].a = input_image[r][c].a; /*A*/
			}
			else
			{
				warped_image[i][j].r = 255; /*R*/
				warped_image[i][j].g = 255; /*G*/
				warped_image[i][j].b = 255; /*B*/
				warped_image[i][j].a = 255; /*A*/
			}		
		}
	}
	
	
	/*flip the image for display*/
	Image_Flip(warped_image,warped_image_flipped,warped_rows,warped_cols);
}


/*twirling of image using s, cX and cY provided by user*/
void Manager::TwirlWarp(double strength, double cX, double cY) 
{	
  // warped image size = original image size
  Vector2D oLeftB;  
  oLeftB.x = 0.0;
  oLeftB.y = 0.0;
 
  Vector2D oLeftT;
  oLeftT.x = 0.0;
  oLeftT.y = double(rows)-1;
  
  Vector2D oRightB;  
  oRightB.x = double(cols)-1;
  oRightB.y = 0.0;
  
  Vector2D oRightT;
  oRightT.x = double(cols)-1; 
  oRightT.y = double(rows)-1; 

  warped_cols = cols;
  warped_rows = rows;
  
  
    if(warped_image)
	{
		if(warped_image[0])
		{
			delete warped_image[0];
		}
		delete warped_image;
	}
	
	if(warped_image_flipped)
	{
		if(warped_image_flipped[0])
		{
			delete warped_image_flipped[0];
		}
		delete warped_image_flipped;
	}
	
	/*allocate memory to store the pixels of the image in RGBA format*/
	warped_image = new pixel*[warped_rows];
	warped_image[0] = new pixel[warped_rows * warped_cols];
	 /*update the pointers to point to each row of the image in continous memory allocation */
	 for (int i = 1; i < warped_rows; i++)
		warped_image[i] = warped_image[i - 1] + warped_cols;
		
	/*allocate memory to store the pixels of the vertically flipped image in RGBA format*/
	warped_image_flipped = new pixel*[warped_rows];
	warped_image_flipped[0] = new pixel[warped_rows * warped_cols];
	/*update the pointers to point to each row of the image in continous memory allocation */
	 for (int i = 1; i < warped_rows; i++)
		warped_image_flipped[i] = warped_image_flipped[i - 1] + warped_cols;

  //origin of warped image is same as origin of original image
  double x = 0;
  double y = 0;

  Matrix3D origin;
  origin[0][0] = x;
  origin[1][0] = y;
  origin[2][0] = 0.0;
  origin[1][1] = 0.0;
  origin[2][2] = 0.0;

  int centerX, centerY;
  centerX = (int)(cX * warped_cols);
  centerY = (int)(cY * warped_rows);
  double minDim = std::min(warped_cols, warped_rows);
  double dstX, dstY, r, angle;

  for(int row = 0; row < warped_rows; ++row)
  {
    for(int col = 0; col < warped_cols; ++col)
    {
      dstX = col - centerX;
      dstY = row - centerY;

      r = sqrt(dstX*dstX + dstY*dstY);
      angle = atan2(dstY , dstX);

      Matrix3D outputPixel;
      outputPixel[0][0] = warped_cols;
      outputPixel[1][0] = warped_rows;
      outputPixel[2][0] = 1.0;
      outputPixel[1][1] = 0.0;
      outputPixel[2][2] = 0.0;

      double uu = r * cos(angle + strength * (r - minDim)/minDim) + centerX;
      double vv = r * sin(angle + strength * (r - minDim)/minDim) + centerY;
      
      int u  = int(uu+0.5);
	  int v  = int(vv+0.5);
			if(v > 0 && v <= rows && u > 0 && u < cols)
			{
				/*handle boundary case*/
				if(u == cols)
					u = cols - 1;
		
				if(v == rows)
					v = rows - 1;
				
				warped_image[row][col].r = input_image[v][u].r; /*R*/
				warped_image[row][col].g = input_image[v][u].g; /*G*/
				warped_image[row][col].b = input_image[v][u].b; /*B*/
				warped_image[row][col].a = input_image[v][u].a; /*A*/
			}
			else
			{
				warped_image[row][col].r = 255; /*R*/
				warped_image[row][col].g = 255; /*G*/
				warped_image[row][col].b = 255; /*B*/
				warped_image[row][col].a = 255; /*A*/
			}
    }
  }
  
  /*flip the image for display*/
	Image_Flip(warped_image,warped_image_flipped,warped_rows,warped_cols);
	
}
