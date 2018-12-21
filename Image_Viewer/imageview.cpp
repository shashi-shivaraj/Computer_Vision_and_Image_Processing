/**********************************************************************
*  FILE NAME	: imageview.cpp
*
*  DESCRIPTION  : Program to display image using OpenGL and GLUT
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  5th Sep,2018         Shashi Shivaraju        CPSC_6040_HW_01
*                       [C88650674]
***********************************************************************/
/*Header file  inclusion*/
#include "imageview.h"
using namespace std;

/*main function of the program*/
int main(int argc,char* argv[])
{
	int ret = 0;	/*varialbe to store return value*/
	int count = 0; /*looping variable*/
	
	/*initialise the GLUT utilities*/
	glutInit(&argc,argv);
	
	/*set window properties*/
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    /*create window*/
    glutCreateWindow("Image Viewer");
    
    /* set the window backgroung to black*/
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);  
	/* flush the OpenGL pipeline to the viewport */
	glFlush();
	
	/*set up the callback routines*/
	glutDisplayFunc(drawImage);	  /* display callback */
    glutKeyboardFunc(handleKey);	  /* keyboard callback */
    glutSpecialFunc(handleSpecialKey); /* arrow keys callback*/
    glutReshapeFunc(handleReshape);   /* window resize callback */
    
    if(argc > (MAX_IMAGES_SUPPORTED + 1))
	{
		std::cout << "Cannot load more than 10 images" << std::endl;
		return 0; 
	}
	else
	{
		/*load images from cmd line and display the first image*/
		for(count = 1;count<argc;count ++)
		{
			ReadImage(argv[count]);
		}
		ReshapeWindow();
		glutPostRedisplay();
	}
	
	/*GLUT event processing infinite loop*/
	glutMainLoop();
	
	return 0;
}
