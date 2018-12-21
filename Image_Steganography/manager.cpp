/**************************************************************************
*  FILE NAME	: manager.cpp
*
*  DESCRIPTION  : Utility functions used to perform image steganography.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  20th Nov,2018        Shashi Shivaraju        CPSC_6040_Final_Project
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
    glutCreateWindow("Steganography Viewer");
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

/*Wrapper function for window display*/
void Manager::wrapperForDisplay() 
{
   Manager::getInstance()->drawImage();
}

/*Wrapper function for window reshape handler*/ 
 void Manager::wrapperForhandleReshape(int w,int h) 
{
   Manager::getInstance()->handleReshape(w,h);
}

/*Function to set the usage of the program*/
void Manager::setUsage(usagemode mode)
{
	usage_mode = mode;
}

/*Function to set the stegonography mode of the program*/
void Manager::setMode(steganomode mode)
{
	stegano_mode = mode;
}

/*default constructor*/
Manager::Manager() 
{
	disp_cols = WINDOW_WIDTH ;  /*width of image in pixels*/
	disp_rows = WINDOW_HEIGHT;  /*height of image in pixels*/
	
	/*initializing all the pointers */
	cover_image = NULL;
	secret_image = NULL;
	secret_image_flipped = NULL;
	hidden_image = NULL;
	hidden_image_flipped = NULL;
	disp_image = NULL;
	scaled_cover_image = NULL;
	compressed_secret_image = NULL;
}

/*creates a singleton class*/
Manager* Manager::getInstance() 
{
  if ( !instance ) instance = new Manager;
  return instance;
}

/*Function to set filenames passed as command line args*/
void Manager::setFilenames(std::string cover,std::string secret,std::string out)
{
	coverfilename = cover;
	secretfilename = secret;
	hiddenfilename = out;
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
	if(disp_image)
	{
			/* set the window background to black*/
			glClearColor(0,0,0,0);
			glClear(GL_COLOR_BUFFER_BIT);
			glRasterPos2i(0,0);
			glDrawPixels(disp_cols,disp_rows,GL_RGBA,GL_UNSIGNED_BYTE,disp_image[0]);
			glutSwapBuffers();
			glutReshapeWindow(disp_cols,disp_rows);
			// flush the OpenGL pipeline to the viewport
			glFlush();
	}
}

/*function to set the window size to fit the image size*/
void Manager::ReshapeWindow()
{
			glutReshapeWindow(disp_cols,disp_rows);
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
		/*deallocate resources before exiting*/
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
	Window Reshape Callback Routine:
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
	 /*code snippet to invert the image upside down and store it in memory pointed by original[0]*/
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

/*Function to deallocate the heap memory*/
void Manager::freeallocations()
{	
	if(cover_image)
	{
		if(cover_image[0])
		{
			delete cover_image[0];
		}
		delete cover_image;
	}
	
	if(secret_image)
	{
		if(secret_image[0])
		{
			delete secret_image[0];
		}
		delete secret_image;
	}
	
	if(secret_image_flipped)
	{
		if(secret_image_flipped[0])
		{
			delete secret_image_flipped[0];
		}
		delete secret_image_flipped;
	}
	
	if(hidden_image)
	{
		if(hidden_image[0])
		{
			delete hidden_image[0];
		}
		delete hidden_image;
	}
	
	if(hidden_image_flipped)
	{
		if(hidden_image_flipped[0])
		{
			delete hidden_image_flipped[0];
		}
		delete hidden_image_flipped;
	}
	
	if(scaled_cover_image)
	{
		if(scaled_cover_image[0])
		{
			delete scaled_cover_image[0];
		}
		delete scaled_cover_image;
	}
	
	if(compressed_secret_image)
	{
		free(compressed_secret_image);
		compressed_secret_image = NULL;
	}	
}

/*Function to implement Steganography operation */
int Manager::PerformSteganography()
{
	int ret = 0;
	if(usage_mode == e_hide) /*Hide the secret image in the cover image to get hidden image*/
	{ 
		/*Read the cover image*/
		ret = ReadImage(e_cover);
		if(ret != 0)
		{
			std::cout<< "ReadImage failed for cover with "<< ret <<std::endl;
			return -1;
		}
		
		/*Read the secret image*/
		ret = ReadImage(e_secret);
		if(ret != 0)
		{
			std::cout<< "ReadImage failed for secret with "<< ret <<std::endl;
			return -1;
		}
		
		if(stegano_mode == e_default)
		{
			/*perform steganography using default method*/
			ret = HideImage_DefaultMode();
			if(ret != 0)
			{
				std::cout<< "HideImage_DefaultMode failed with "<< ret <<std::endl;
				return -1;
			}
		}
		else if(stegano_mode == e_compressed)
		{
			/*perform steganography using compression method*/
			ret = HideImage_CompressionMode();
			if(ret != 0)
			{
				std::cout<< "HideImage_CompressionMode failed with "<< ret <<std::endl;
				return -1;
			}
		}
		
		/*Write the output image to file*/
		ret = WriteImage(e_hidden);
		if(ret != 0)
		{
			std::cout<< "WriteImage failed for hidden with "<< ret <<std::endl;
			return -1;
		}
		
		/* Display the output image*/
		disp_image = hidden_image_flipped;
		disp_cols = hidden_cols;
		disp_rows = hidden_rows;
	}
	else if(usage_mode == e_unhide)  /*Extract the secret image from the hidden image*/
	{
		/*read the steganographed image*/
		ret = ReadImage(e_hidden);
		if(ret != 0)
		{
			std::cout<< "ReadImage failed for hidden with "<< ret <<std::endl;
			return -1;
		}
		
		if(stegano_mode == e_default)
		{
			/*perform reverse steganography using default method*/
			ret = UnHideImage_DefaultMode();
			if(ret != 0)
			{
				std::cout<< "HideImage_CompressionMode failed with "<< ret <<std::endl;
				return -1;
			}
		}
		else if(stegano_mode == e_compressed)
		{
			/*perform reverse steganography using compression method*/
			ret = UnHideImage_CompressionMode();
			if(ret != 0)
			{
				std::cout<< "HideImage_CompressionMode failed with "<< ret <<std::endl;
				return -1;
			}
		}
		
		/*Write the output image to file*/
		
		ret = WriteImage(e_cover);
		if(ret != 0)
		{
			std::cout<< "WriteImage failed for cover with "<< ret <<std::endl;
			return -1;
		}
		
		ret = WriteImage(e_secret);
		if(ret != 0)
		{
			std::cout<< "WriteImage failed for hidden with "<< ret <<std::endl;
			return -1;
		}
		
		/* Display the output image*/
		disp_image = secret_image_flipped;
		disp_cols = secret_cols;
		disp_rows = secret_rows;
		
	}

	return 0;
}

/*Function to open image file and store its data in a pixel map*/
int Manager::ReadImage(filetype file_mode)
{
	FILE *fp = NULL;
	std::string filename;
	unsigned char *tempImage = NULL; /*temporary pointer to store the pixel map*/
	pixel **input_image = NULL;
	pixel **input_image_flipped = NULL;
	
	int ret = 0; /*variable to check return value*/ 
	int count = 0,i=0,j=0; /*looping variables*/
	int rows=0,cols=0;
	
	/*determine the file to read*/
	switch(file_mode)
	{
		case e_cover:
		{
			filename = coverfilename;
		}
		break;
		case e_secret:
		{
			filename = secretfilename;
		}
		break;
		case e_hidden:
		{
			filename = hiddenfilename;
		}
		break;
		default:
		{
			std::cout<<"invalid file mode"<<std::endl;
			return -1;
		}
	}
	
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
	std::cout <<"Loaded "<<filename <<std::endl;
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
	
	/*store the image data into memory*/
	image->read_image(TypeDesc::UINT8,tempImage);
	
	/*allocate memory to store the pixels of the image in RGBA format*/
	input_image = new pixel*[rows];
	input_image[0] = new pixel[rows * cols];
	 /*update the pointers to point to each row of the image in continous memory allocation */
	 for (int i = 1; i < rows; i++)
		input_image[i] = input_image[i - 1] + cols;
		
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
	
	/*update according to the filetype read*/
	switch(file_mode)
	{
		case e_cover: /*this will occur in image merging*/ 
		{
			cover_image = input_image;
			cover_rows = rows;
			cover_cols = cols;
		}
		break;
		case e_secret: /*this will occur in image merging*/ 
		{	
			secret_image = input_image;
			secret_rows = rows;
			secret_cols = cols;
		}
		break;
		case e_hidden:/*this will occur only in image extraction*/
		{
			hidden_image = input_image;
			hidden_rows = rows;
			hidden_cols = cols;
		}
		break;
		default:
		{
			std::cout<<"invalid file mode"<<std::endl;
			return -1;
		}
	}
	
	/*close the file*/
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

/*Function to Merge the image in compression mode*/
int Manager::HideImage_CompressionMode()
{
	int ret = 0;
	int secret_image_size = secret_cols*secret_rows*RGBA_CHANNELS;
	int avail_cover_size = cover_cols*cover_rows; /*each pixel can store one byte r=3bits g=3bits and b = 2bits*/
	int required_size = 0;
	
	/*allocate memory to store secret image with its image info (4bytes for secret_cols+secret_rows + secret_image_size bytes)*/
	unsigned char *raw_data = (unsigned char *)calloc(secret_image_size + 4,sizeof(unsigned char));
	if(!raw_data)
	{
		std::cout<<"memory allocation failed"<<std::endl;
		return -1;
	}
	
	memcpy(raw_data,(unsigned short*)&secret_cols,sizeof(unsigned short)); //2bytes for cols
	memcpy(raw_data+sizeof(unsigned short),(unsigned short*)&secret_rows,sizeof(unsigned short)); //2bytes for rows
	memcpy(raw_data+sizeof(unsigned short)*2,(unsigned char *)secret_image[0],secret_image_size); //rest is secret image data
	
	/*Encode the secert image data using huffman encoding*/
	compressed_secret_image = huffman_encode((unsigned char *)raw_data,secret_image_size+4,compressed_secret_size);
	if(!compressed_secret_image)
	{
		printf("huffman_encode failed\n");
		return -1;
	}
	
#ifdef DEBUG_MODE
	/*write the raw data to a file*/
	FILE *encode_fp = fopen("secret_raw.dump","wb");
	if(!encode_fp)
	{
		printf("fopen failed for secret_rgba.dump");
		return -1;
	}
	
	/*Write encoded data  to file*/
	ret = fwrite(raw_data,1,secret_image_size+4,encode_fp);
	if(ret != secret_image_size+4)
	{
		printf("fwrite failed with %d\n",ret);
	}
	
	if(encode_fp)
	{
		fclose(encode_fp);
		encode_fp = NULL;
	}	
#endif /*DEBUG_MODE*/
	
	/*free the heap memory*/
	if(raw_data)
	 free(raw_data);
	raw_data = NULL;

#ifdef DEBUG_MODE
	printf("secret_image_size = %d bytes\n",secret_image_size); 
	printf("compressed secret image size = %d bytes\n",compressed_secret_size+4);/*4 bytes for header info*/
	printf("space available in cover_image = %d bytes\n",avail_cover_size);

	/*write the huffman encoded data to a file*/
	FILE *encode_fpt = fopen("secret_encoded.dump","wb");
	if(!encode_fpt)
	{
		printf("fopen failed for secret_rgba.dump");
		return -1;
	}
	
	/*Write encoded data  to file*/
	ret = fwrite(compressed_secret_image,1,compressed_secret_size,encode_fpt);
	if(ret != compressed_secret_size)
	{
		printf("fwrite failed with %d\n",ret);
	}
	
	if(encode_fpt)
	{
		fclose(encode_fpt);
		encode_fpt = NULL;
	}	
#endif /*DEBUG_MODE*/

	required_size = compressed_secret_size + RGBA_CHANNELS; /*1 pixel for compressed data length*/
	if(avail_cover_size >= required_size ) 
	{
		/*cover image can accomodate the secret compressed image data*/
			
			/*update the size of the hidden image*/
			hidden_cols = cover_cols;
			hidden_rows = cover_rows;
			/*Merge the secret compressed image into the cover image
			 * Each pixel of cover image will contain one byte of the 
			 * compressed data of secret image*/
			MergeImages_CompressionMode();	
	}
	else
	{
			int extra_required = required_size - avail_cover_size;
			
#ifdef DEBUG_MODE
	printf("extra_required pixels required = %d bytes\n",extra_required);
#endif /*DEBUG_MODE*/
			/*print error msg to user*/
			std::cout<<"cannot accomodate compressed secret image in the cover image"<<std::endl;
			std::cout<<"Needs considerable reduction in dimensions of secret image and increase in dimensions of cover image"<<std::endl;
			return -1;			
	}
	
	return 0;
}

/*Function to Merge the image in Default mode 
 * i.e replace LSB of cover pixels by MSB of secret pixels*/
int Manager::HideImage_DefaultMode()
{
	int ret = 0;
	int cover_size = 0,secret_size = 0;
	int i = 0,config_bits = BITS_TO_HIDE;
	
	/*size of images in bytes*/
	cover_size = cover_cols*cover_rows*RGBA_CHANNELS; 
	secret_size = secret_cols*secret_rows*RGBA_CHANNELS;
	
	if(secret_size < (cover_size - ENCRYPTION_HEADER_SIZE)) /*cover image can accomodate the secret image*/
	{
		/*update the size of the hidden image*/
		hidden_cols = cover_cols;
		hidden_rows = cover_rows;
		/*hide the secret image in cover image*/
		MergeImages(config_bits);		
	}
	else if(secret_size >= (cover_size - ENCRYPTION_HEADER_SIZE)) /*scale the cover image to accomodate the secret image  */
	{
		int new_cover_cols = secret_cols+1,new_cover_rows = secret_rows+1;
		float scalex = new_cover_cols/(float)cover_cols,scaley = new_cover_rows/(float)cover_rows;
		
		std::cout<<"cover image scaled to accomodate the secret image with scalex = "<<scalex<< " scaley = "<<scaley<<std::endl;
		ScaleCoverImage(scalex,scaley);
		/*update the size of the hidden image*/
		hidden_cols = cover_cols;
		hidden_rows = cover_rows;
		/*hide the secret image in cover image*/
		MergeImages(config_bits);			
	}
	
	return ret;
}

/*Function to extract the secret image in default mode*/
int Manager::UnHideImage_DefaultMode()
{
	int i = 0;
	unsigned short header = 0,temp = 0;
	
	/*update the cover image dimension*/
	cover_cols = hidden_cols;
	cover_rows = hidden_rows;
	
	/*parse the first pixel of hidden image to get secret image width*/
	header = (hidden_image[0][0].r & 0x00FF);/*stores LSB of cols*/
	temp = (hidden_image[0][0].g & 0x00FF)<<8;/*stores MSB of cols*/
	secret_cols = (unsigned int)(temp|header);

	/*parse the second pixel of hidden image to get secret image height*/
	header = (hidden_image[0][1].r & 0x00FF);/*stores LSB of rows*/
	temp = (hidden_image[0][1].g & 0x00FF)<<8;/*stores MSB of rows*/
	secret_rows = (unsigned int)(temp|header);  

#ifdef DEBUG_MODE
	std::cout<<"secret_cols = "<<secret_cols<<" r = "<<(unsigned int)hidden_image[0][0].r<<" g = "<<(unsigned int)hidden_image[0][0].g<<std::endl;
	std::cout<<"secret_rows = "<<secret_rows<<" r = "<<(unsigned int)hidden_image[0][1].r<<" g= "<<(unsigned int)hidden_image[0][1].g<<std::endl;
#endif /*DEBUG_MODE*/
	
	/*allocate memory to store the pixels of the cover image in RGBA format*/
	cover_image = new pixel*[cover_rows];
	cover_image[0] = new pixel[cover_rows * cover_cols];
	 /*update the pointers to point to each row of the image in continous memory allocation */
	 for (i = 1; i < cover_rows; i++)
		cover_image[i] = cover_image[i - 1] + cover_cols;
		
	/*allocate memory to store the pixels of the secert image in RGBA format*/
	secret_image = new pixel*[secret_rows];
	secret_image[0] = new pixel[secret_rows * secret_cols];
	 /*update the pointers to point to each row of the image in continous memory allocation */
	 for (i = 1; i < secret_rows; i++)
		secret_image[i] = secret_image[i - 1] + secret_cols;
		
	/*allocate memory to store the pixels of the vertically flipped hidden image in RGBA format*/
	secret_image_flipped = new pixel*[secret_rows];
	secret_image_flipped[0] = new pixel[secret_rows * secret_cols];
	/*update the pointers to point to each row of the image in continous memory allocation */
	 for (int i = 1; i < secret_rows; i++)
		secret_image_flipped[i] = secret_image_flipped[i - 1] + secret_cols;
	
	for(i=0;i<secret_cols*secret_rows;i++)
	{
		/*3 bits of byte = pixel value of secret image*/
		secret_image[0][i].r = (hidden_image[0][i+2].r & 0x07) << 5;
		secret_image[0][i].g = (hidden_image[0][i+2].g & 0x07) << 5;
		secret_image[0][i].b = (hidden_image[0][i+2].b & 0x07) << 5;
		secret_image[0][i].a = hidden_image[0][i+2].a;
		
		/*5 bits of byte = pixel value of cover image*/
		if(i==0 || i ==1) /*first two bytes contained image dimension so use the adjacent pixel color*/
		{
			cover_image[0][i].r = hidden_image[0][2].r & 0xF8;
			cover_image[0][i].g = hidden_image[0][2].g & 0xF8;
			cover_image[0][i].b = hidden_image[0][2].b & 0xF8;
			cover_image[0][i].a = hidden_image[0][2].a;
		}
		else
		{
			cover_image[0][i].r = hidden_image[0][i].r & 0xF8;
			cover_image[0][i].g = hidden_image[0][i].g & 0xF8;
			cover_image[0][i].b = hidden_image[0][i].b & 0xF8;
			cover_image[0][i].a = hidden_image[0][i].a;	
		}
	}
	
	/*rest of the pixels are without the secret image info*/
	for(i=secret_cols*secret_rows;i<cover_rows * cover_cols;i++)
	{
		cover_image[0][i].r = hidden_image[0][i].r;
		cover_image[0][i].g = hidden_image[0][i].g;
		cover_image[0][i].b = hidden_image[0][i].b;
		cover_image[0][i].a = hidden_image[0][i].a;
	}	

	
	/*flip the extracted image for display purpose*/
	Image_Flip(secret_image,secret_image_flipped,secret_rows,secret_cols);
	
	return 0;
}

/*Function to extract the secret image in default mode*/
int Manager::UnHideImage_CompressionMode()
{
	int i = 0,msg_index = 0;
	unsigned char msg = 0;
	unsigned char* dim_with_secret_image_data = NULL;
	unsigned int   dim_with_secret_image_size = 0;
	
	/*update the cover image dimension*/
	cover_cols = hidden_cols;
	cover_rows = hidden_rows;
	
	/*parse the first pixel of hidden image to get secret image compressed datalength*/
	compressed_secret_size = (unsigned int)(hidden_image[0][0].r & 0x000000FF);/*stores minor LSB of size*/
	compressed_secret_size = (unsigned int)(compressed_secret_size | ((hidden_image[0][0].g & 0x000000FF)<<8));/*store major LSB of size*/
	compressed_secret_size = (unsigned int)(compressed_secret_size | ((hidden_image[0][0].b & 0x000000FF)<<16));/*store minor MSB of size*/
	/*This alpha byte will work in bmp but not in png 
	 but logically 24bits must be sufficient for compressed image size */
	//compressed_secret_size = (unsigned int)(compressed_secret_size | ((hidden_image[0][0].b & 0x000000FF)<<24));/*store major MSB of size*/
	
#ifdef DEBUG_MODE
	printf("r = %d g = %d b = %d\n",hidden_image[0][0].r,hidden_image[0][0].g,hidden_image[0][0].b);
	std::cout<<"compressed secret image size in bytes = "<<compressed_secret_size<<std::endl;
#endif /*DEBUG_MODE*/

	/*allocate memory to store the encrypted secret image data*/
	compressed_secret_image = (unsigned char *)calloc(compressed_secret_size,sizeof(unsigned char));
	if(!compressed_secret_image)
	{
		printf("memory allocation failed\n");
		return -1;
	}
	
	/*allocate memory to store the pixels of the cover image in RGBA format*/
	cover_image = new pixel*[cover_rows];
	cover_image[0] = new pixel[cover_rows * cover_cols];
	 /*update the pointers to point to each row of the image in continous memory allocation */
	 for (i = 1; i < cover_rows; i++)
		cover_image[i] = cover_image[i - 1] + cover_cols;
	
	/*Extract the huffman encoded data from the image
	 * Each pixel contains one byte of the encoded data (r = 3 bits;g = 3 bits;b = 2 bits)*/
	msg_index = 0;
	for(i=0;i<hidden_cols*hidden_rows;i++)
	{
		if(msg_index < compressed_secret_size) /*parse until the all the encoded data is obtained*/
		{
			if( i!= 0)
			{
				/*(r = 5 bits;g = 5 bits;b = 6 bits)*/
				cover_image[0][i].r = hidden_image[0][i].r & 0xF8; /*discard 3 bits*/
				cover_image[0][i].g = hidden_image[0][i].g & 0xF8; /*discard 3 bits*/
				cover_image[0][i].b = hidden_image[0][i].b & 0xFC; /*discard 2 bits*/
				cover_image[0][i].a = hidden_image[0][i].a;
				
				/*(r = 3 bits;g = 3 bits;b = 2 bits)*/
				msg = (hidden_image[0][i].r & 0x07)|(hidden_image[0][i].g & 0X07)<<3|(hidden_image[0][i].b & 0X03)<<6;
				compressed_secret_image[msg_index] = msg;
			
				msg_index ++; /*increment to access next byte*/
			}
			else /*First pixel contained the total length of the enocoded data*/
			{
				cover_image[0][i].r = hidden_image[0][i+1].r & 0xF8; /*discard 3 bits*/
				cover_image[0][i].g = hidden_image[0][i+1].g & 0xF8; /*discard 3 bits*/
				cover_image[0][i].b = hidden_image[0][i+1].b & 0xFC; /*discard 2 bits*/
				cover_image[0][i].a = hidden_image[0][i+1].a;
				
			}
		}
		else /*rest all pixels donot contain the huffman encoded data*/
		{
			cover_image[0][i].r = hidden_image[0][i].r ;
			cover_image[0][i].g = hidden_image[0][i].g ;
			cover_image[0][i].b = hidden_image[0][i].b ;
			cover_image[0][i].a = hidden_image[0][i].a;
		}
	}

#ifdef DEBUG_MODE
	printf("compressed secret image size = %d bytes\n",compressed_secret_size);
	/*write the exrtracted huffman encoded data to a file*/
	FILE *encode_fpt = fopen("unhide_secret_encoded.dump","wb");
	if(!encode_fpt)
	{
		printf("fopen failed for secret_rgba.dump");
		return -1;
	}
	
	/*Write encoded data  to file*/
	int ret = fwrite(compressed_secret_image,1,compressed_secret_size,encode_fpt);
	if(ret != compressed_secret_size)
	{
		printf("fwrite failed with %d\n",ret);
	}
	
	if(encode_fpt)
	{
		fclose(encode_fpt);
		encode_fpt = NULL;
	}	
#endif /*DEBUG_MODE*/
	
	/*decode the huffman encoded data*/
	dim_with_secret_image_data = huffman_decode(compressed_secret_image,
												compressed_secret_size,dim_with_secret_image_size);

	if(!dim_with_secret_image_data)
	{
		printf("huffman_decode failed\n");
		return -1;
	}
	
	/*first four bytes repesents the dimension of the secret image*/
	secret_cols = (unsigned int) (dim_with_secret_image_data[0]|(dim_with_secret_image_data[1]<<8));
	secret_rows = (unsigned int) (dim_with_secret_image_data[2]|(dim_with_secret_image_data[3]<<8));
	
#ifdef DEBUG_MODE
	printf("Secret image dimension : secret_cols = %d secret_rows =%d\n",secret_cols,secret_rows);
#endif /*DEBUG_MODE*/		

	/*allocate memory to store the pixels of the secert image in RGBA format*/
	secret_image = new pixel*[secret_rows];
	secret_image[0] = new pixel[secret_rows * secret_cols];
	 /*update the pointers to point to each row of the image in continous memory allocation */
	 for (i = 1; i < secret_rows; i++)
		secret_image[i] = secret_image[i - 1] + secret_cols;
		
	/*allocate memory to store the pixels of the vertically flipped hidden image in RGBA format*/
	secret_image_flipped = new pixel*[secret_rows];
	secret_image_flipped[0] = new pixel[secret_rows * secret_cols];
	/*update the pointers to point to each row of the image in continous memory allocation */
	 for (int i = 1; i < secret_rows; i++)
		secret_image_flipped[i] = secret_image_flipped[i - 1] + secret_cols;
	
	/*copy secret image data in RGBA format*/
	memcpy((unsigned char *)secret_image[0],dim_with_secret_image_data + 4,secret_rows * secret_cols * RGBA_CHANNELS);
	
	/*free the heap memory*/
	if(dim_with_secret_image_data)
		free(dim_with_secret_image_data);
	dim_with_secret_image_data = NULL;

	/*flip the image for display*/
	Image_Flip(secret_image,secret_image_flipped,secret_rows,secret_cols);
	return 0;
}

/*Function to merge the secret image inside the cover image in default mode*/
void Manager::MergeImages(int config_bits)
{
	int i = 0;
	unsigned short header = 0;
	
	/*allocate memory to store the pixels of the hidden image in RGBA format*/
	hidden_image = new pixel*[hidden_rows];
	hidden_image[0] = new pixel[hidden_rows * hidden_cols];
	 /*update the pointers to point to each row of the image in continous memory allocation */
	 for (int i = 1; i < hidden_rows; i++)
		hidden_image[i] = hidden_image[i - 1] + hidden_cols;
		
	/*allocate memory to store the pixels of the vertically flipped hidden image in RGBA format*/
	hidden_image_flipped = new pixel*[hidden_rows];
	hidden_image_flipped[0] = new pixel[hidden_rows * hidden_cols];
	/*update the pointers to point to each row of the image in continous memory allocation */
	 for (int i = 1; i < hidden_rows; i++)
		hidden_image_flipped[i] = hidden_image_flipped[i - 1] + hidden_cols;
		
	
	/*store the secret image width in the first pixel of the hidden image*/
	header = (unsigned short)secret_cols;
	hidden_image[0][0].r = (unsigned char)(header & 0x00FF); /*store LSB of cols*/
	hidden_image[0][0].g = (unsigned char)((header & 0xFF00)>>8); /*store MSB of cols*/
	hidden_image[0][0].b = cover_image[0][0].b;
	hidden_image[0][0].a = cover_image[0][0].a;
		/*store the secret image height in the second pixel of the hidden image*/
	header = (unsigned short)secret_rows;
	hidden_image[0][1].r = (unsigned char)(header & 0x00FF); /*store LSB of rows*/;
	hidden_image[0][1].g = (unsigned char)((header & 0xFF00)>>8);/*store MSB of cols*/
	hidden_image[0][1].b = cover_image[0][1].b;
	hidden_image[0][1].a = cover_image[0][1].a;

#ifdef DEBUG_MODE
	std::cout<<"secret_cols = "<<secret_cols<<" r = "<<(unsigned int)hidden_image[0][0].r<<" g = "<<(unsigned int)hidden_image[0][0].g<<std::endl;
	std::cout<<"secret_rows = "<<secret_rows<<" b = "<<(unsigned int)hidden_image[0][1].r<<" g= "<<(unsigned int)hidden_image[0][1].g<<std::endl;
#endif /*DEBUG_MODE*/

	/*Replace the 3 bits LSB cover pixel with 3bit MSB of secret image*/
 	for(i=0;i<secret_cols*secret_rows;i++)
	{
		hidden_image[0][i+2].r = (cover_image[0][i+2].r & 0xF8) | ((secret_image[0][i].r & 0xE0)>>5);
		hidden_image[0][i+2].g = (cover_image[0][i+2].g & 0xF8) | ((secret_image[0][i].g & 0xE0)>>5);
		hidden_image[0][i+2].b = (cover_image[0][i+2].b & 0xF8) | ((secret_image[0][i].b & 0xE0)>>5);
		hidden_image[0][i+2].a = cover_image[0][i+2].a;
	}
	
	/*rest all pixels donot contain any secret image pixels*/
	for(i=secret_cols*secret_rows;i<hidden_cols*hidden_rows;i++)
	{
		hidden_image[0][i].r = cover_image[0][i].r;
		hidden_image[0][i].g = cover_image[0][i].g;
		hidden_image[0][i].b = cover_image[0][i].b;
		hidden_image[0][i].a = cover_image[0][i].a;
	}	
	
	/*flip the image for display*/
	Image_Flip(hidden_image,hidden_image_flipped,hidden_rows,hidden_cols);
}

/*Function to merge the secret image inside the cover image in compression mode*/
void Manager::MergeImages_CompressionMode()
{
	int i = 0;
	unsigned int header = 0,msg_index = 0;
	unsigned char msg = 0;
	
	/*allocate memory to store the pixels of the hidden image in RGBA format*/
	hidden_image = new pixel*[hidden_rows];
	hidden_image[0] = new pixel[hidden_rows * hidden_cols];
	 /*update the pointers to point to each row of the image in continous memory allocation */
	 for (int i = 1; i < hidden_rows; i++)
		hidden_image[i] = hidden_image[i - 1] + hidden_cols;
		
	/*allocate memory to store the pixels of the vertically flipped hidden image in RGBA format*/
	hidden_image_flipped = new pixel*[hidden_rows];
	hidden_image_flipped[0] = new pixel[hidden_rows * hidden_cols];
	/*update the pointers to point to each row of the image in continous memory allocation */
	 for (int i = 1; i < hidden_rows; i++)
		hidden_image_flipped[i] = hidden_image_flipped[i - 1] + hidden_cols;
		
	
	/*store the secret image size in the first pixel of the hidden image*/
	header = compressed_secret_size;
	hidden_image[0][0].r = (unsigned char)(header & 0x000000FF); /*store minor LSB of size*/
	hidden_image[0][0].g = (unsigned char)((header & 0x0000FF00)>>8); /*store major LSB of size*/
	hidden_image[0][0].b = (unsigned char)((header & 0x00FF0000)>>16); /*store minor MSB of size*/
	/*logically 24bits must be sufficient for compressed image size */
	hidden_image[0][0].a =255;
	
#ifdef DEBUG_MODE
	std::cout<<"secret_compressed data size = "<<compressed_secret_size<<" r = "<<(unsigned int)hidden_image[0][0].r<<" g= "<<(unsigned int)hidden_image[0][0].g<<" b = "<<(unsigned int)hidden_image[0][0].b<<" a = "<<(unsigned int)hidden_image[0][0].a<<std::endl;
#endif /*DEBUG_MODE*/

 	for(i=0;i<hidden_cols*hidden_rows;i++)
	{
		if(msg_index < compressed_secret_size)
		{
			msg = compressed_secret_image[msg_index]; /*one byte at a time*/
		
			/*store one byte of huffman encoded data in each pixel*/
			hidden_image[0][i+1].r = (cover_image[0][i+1].r & 0xF8) | (msg & 0X07); /*3 bits in red*/
			hidden_image[0][i+1].g = (cover_image[0][i+1].g & 0xF8) | ((msg & 0X38)>>3); /*3 bits in green*/
			hidden_image[0][i+1].b = (cover_image[0][i+1].b & 0xF8) | ((msg & 0XC0)>>6);; /*2 bits in blue*/
			hidden_image[0][i+1].a = cover_image[0][i+1].a;
			
			msg_index ++; /*increment to access next byte*/
		}
		else /*reset all pixels donot contain the encoded data*/
		{
			hidden_image[0][i+1].r = cover_image[0][i+1].r;
			hidden_image[0][i+1].g = cover_image[0][i+1].g;
			hidden_image[0][i+1].b = cover_image[0][i+1].b;
			hidden_image[0][i+1].a = cover_image[0][i+1].a;
		}
	}	
	
	/*flip the image for display*/
	Image_Flip(hidden_image,hidden_image_flipped,hidden_rows,hidden_cols);
}

/*
    Routine to write the pixelmap to an image file
*/
int Manager::WriteImage(filetype file_mode)
{

  /*variable declarations*/
  int width = 0;
  int height = 0;
  pixel **pixmap = NULL;
  std::string outputfilename ;
    
  /*determine the image file to be written*/
  switch(file_mode)
	{
		case e_cover:
		{
			outputfilename = coverfilename;
			width  = cover_cols;
			height = cover_rows;
			pixmap = cover_image;
		}
		break;
		case e_secret:
		{
			outputfilename = secretfilename;
			width  = secret_cols;
			height = secret_rows;
			pixmap = secret_image;
		}
		break;
		case e_hidden:
		{
			outputfilename = hiddenfilename;
			width  = hidden_cols;
			height = hidden_rows;
			pixmap = hidden_image;
		}
		break;
		default:
		{
			std::cout<<"invalid file mode"<<std::endl;
			return -1;
		}
	}
		
	/*return if pixelmap is not ready*/
	if(!pixmap)
	{
		std::cout<<"image not loaded for file storage"<<std::endl;
		return -1;
	}

  /* create the oiio file handler for the image*/
  ImageOutput *outfile = ImageOutput::create(outputfilename);
  if(!outfile){
    std::cerr << "Could not create output image for " << outputfilename << ", error = " << geterror() << std::endl;
    return -1;
  }
   
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

  /* write the image to the file */
  if(!outfile->write_image(TypeDesc::UINT8,pixmap[0])) /*default z stride*/
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


/*Function to calculate corner points and dimension of the scaled image using Forward transformation*/
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
	Corner2.y = cover_rows;
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
	Corner3.x = cover_cols;
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
	Corner4.x = cover_cols;
	Corner4.y = cover_rows;
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
	
	
	scaled_cover_cols = int (maxX - minX + 0.5);
	scaled_cover_rows = int (maxY - minY + 0.5);

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
	scaled_cover_xycorners[0].x = NCorner1.x/NCorner1.z;
	scaled_cover_xycorners[0].y = NCorner1.y/NCorner1.z;
	
	/*corner = (0,H)*/
	NCorner2.y = cover_rows;
	NCorner2 = M * NCorner2;
	scaled_cover_xycorners[1].x = NCorner2.x/NCorner2.z;
	scaled_cover_xycorners[1].y = NCorner2.y/NCorner2.z;
	
	/*corner = (W,0)*/
	NCorner3.x = cover_cols;
	NCorner3 = M * NCorner3;
	scaled_cover_xycorners[3].x = NCorner3.x/NCorner3.z;
	scaled_cover_xycorners[3].y = NCorner3.y/NCorner3.z;
	
	/*corner = (W,H)*/
	NCorner4.x = cover_cols;
	NCorner4.y = cover_rows;
	NCorner4 = M * NCorner4;
	scaled_cover_xycorners[2].x = NCorner4.x/NCorner4.z;
	scaled_cover_xycorners[2].y = NCorner4.y/NCorner4.z;
	
	/*allocate memory to store the pixels of the scaled image in RGBA format*/
	scaled_cover_image = new pixel*[scaled_cover_rows];
	scaled_cover_image[0] = new pixel[scaled_cover_rows * scaled_cover_cols];
	 /*update the pointers to point to each row of the scaled image in continous memory allocation */
	 for (int i = 1; i < scaled_cover_rows; i++)
		scaled_cover_image[i] = scaled_cover_image[i - 1] + scaled_cover_cols;
			
	return;
}


/*Function to obtain the scaled cover image using Inverse Transform*/
void Manager::InverseTransform(Matrix3D invM)
{
	int i = 0,j = 0;
	int r = 0,c = 0;
	Vector3D inputPixelLocation;
	Vector3D outputPixelLocation;
	
	/*map each pixel of input image to the output warped image*/
	for(i=0;i<scaled_cover_rows;i++)
	{
		for(j=0;j<scaled_cover_cols;j++)
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
			
			if(r >= 0 && r <= cover_rows && c >= 0 && c <= cover_cols)
			{
				/*handle boundary case*/
				if(c == cover_cols)
					c = cover_cols - 1;
		
				if(r == cover_rows)
					r = cover_rows - 1;
				
				scaled_cover_image[i][j].r = cover_image[r][c].r; /*R*/
				scaled_cover_image[i][j].g = cover_image[r][c].g; /*G*/
				scaled_cover_image[i][j].b = cover_image[r][c].b; /*B*/
				scaled_cover_image[i][j].a = cover_image[r][c].a; /*A*/
			}
			else /*handle boundary case*/
			{
				if(r >= cover_rows)
					r = cover_rows - 1;
					
				if(r < 0)
					r = 0;
					
				if(c >= cover_cols)
					c = cover_cols - 1;
				
				if(c < 0)
					c = 0;
					
				scaled_cover_image[i][j].r = cover_image[r][c].r; /*R*/
				scaled_cover_image[i][j].g = cover_image[r][c].g; /*G*/
				scaled_cover_image[i][j].b = cover_image[r][c].b; /*B*/
				scaled_cover_image[i][j].a = cover_image[r][c].a; /*A*/
			}
		}
	}
}

/*Function to scale the cover image to required dimension*/
void Manager::ScaleCoverImage(float scalex,float scaley)
{
		//initialize transformation matrix to identity
		Matrix3D M;
		Matrix3D invM;
		
		/*apply scale factors to the transformation matrix*/
		Scale(M,scalex,scaley);
		
		/*calculate the size of the output warped cover image using forward transform*/
		ForwardTransform(M);
		
		/*calculate the inverse transform*/
		invM = M.inverse();
		
		/*Perform inverse transform to get the warped image*/
		InverseTransform(invM);
		
		/*free the original cover image*/
		if(cover_image)
		{
			if(cover_image[0])
			{
				delete cover_image[0];
			}
			delete cover_image;
		}
		
		/*update the pointer and dimensions of the cover image*/
		cover_image = scaled_cover_image;
		scaled_cover_image = NULL;
		
		cover_cols = scaled_cover_cols;
		cover_rows = scaled_cover_rows;
}
