/**************************************************************************
*  FILE NAME	: compose.cpp
*
*  DESCRIPTION  : Program to perform compositing of two images.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  2nd Oct,2018         Shashi Shivaraju        CPSC_6040_HW_03
*                       [C88650674]
***************************************************************************/
/*Header file  inclusion*/
#include "compose.h"

int main(int argc,char*argv[])
{
	imageinfo* foreground_image = NULL;
	imageinfo* background_image = NULL;
	imageinfo* composite_image = NULL;
	
	int ret = 0;	/*varialbe to store return value*/
	int count = 0; /*looping variable*/
	
	/*check for valid command line args*/
	if(3 > argc)
	{
		std::cout<<"usage:./[exe] [foreground.png] [background.img] [optional out.img]"<<std::endl;
		return -1;
	}
	
	/*initialise the GLUT utilities*/
	glutInit(&argc,argv);
	
	/*set window properties*/
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    /*create window*/
    glutCreateWindow("Composition Viewer");
    
    /* set the window backgroung to black*/
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);  
	/* flush the OpenGL pipeline to the viewport */
	glFlush();
	
	/*set up the callback routines*/
	glutDisplayFunc(drawImage);	  /* display callback */
    glutKeyboardFunc(handleKey);	  /* keyboard callback */
    glutReshapeFunc(handleReshape);   /* window resize callback */
    
   
	/*read the foreground image*/
	foreground_image = ReadImage(argv[1]);
	if(!foreground_image)
	{
		std::cout<<"ReadImage failed for input image : "<< argv[1] <<std::endl;
		return -1;
	}
	
	/*read the background image*/
	background_image = ReadImage(argv[2]);
	if(!background_image)
	{
		std::cout<<"ReadImage failed for input image : "<< argv[2] <<std::endl;
		return -1;
	}
	
	/*compute the composite image with foreground over background*/
	composite_image = ImageComposition(foreground_image,background_image);
	if(!composite_image)
	{
		std::cout<<"ImageComposition failed "<<std::endl;
		return -1;
	}
	
	if(argc > 3)
	{
		std::string outfile(argv[3]);
		/*write the composite image to file*/
		ret = WriteImage(outfile,composite_image);
		if(ret != 0)
		{
			std::cout<<"WriteImage failed "<<std::endl;
		} 
	}
	
	/*display the composition image in window*/
	ReshapeWindow();
	glutPostRedisplay();
	
	/*GLUT event processing infinite loop*/
	glutMainLoop();
	
	return 0;
}
