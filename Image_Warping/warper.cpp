/**************************************************************************
*  FILE NAME	: warper.cpp
*
*  DESCRIPTION  : Program to perform image warping
* 				  and to display the image.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  27th Oct,2018        Shashi Shivaraju        CPSC_6040_HW_05
*                       [C88650674]
* Reference :Ioannis Karamouzas, 10/15/18
***************************************************************************/
/*Header file inclusion*/
#include "manager.h"

/*Have used a singleton design pattern to remove use of global variables*/
Manager* Manager::instance = NULL;

/*
   Main program to read an image file, then ask the user
   for transform information, transform the image and display
   it using the appropriate warp.  Optionally save the transformed
   images in  files.
*/
int main(int argc, char *argv[])
{
	int ret = 0;
	std::string input,out;
		
	/*check for valid command line args*/
	if(argc < 2)
	{
		std::cout<<"Usage:./warper input.img [optional output.img]"<<std::endl;
		return -1;
	}
	
	input = argv[1];
	if(argc == 3)
	{
		out = argv[2]; 
	}
	
	// initialize transformation matrix to identity
	Matrix3D M;
	Matrix3D invM;

	/*Create a Manager*/
	Manager* manager = Manager::getInstance();	
	/*Set file names*/
	manager->setFilenames(input,out);

   //your code to read in the input image
   /*Read the input image and display it*/
	ret = manager->ReadImage(input);
	if(ret != 0)
	{
		std::cout<<"Read Image failed"<<std::endl;
		return ret;
	}

#ifdef DEBUG_MODE	
	// printout the initial matrix
    std::cout << "Initial Matrix: " << std::endl;
    M.print();
#endif /*DEBUG_MODE*/
    
   //build the transformation matrix based on user input
   read_input(M,manager->warpMode);

#ifdef DEBUG_MODE	
	// printout the matrix
    std::cout << "Accumulated Matrix (Forward Map M = ): " << std::endl;
    M.print();
#endif /*DEBUG_MODE*/
    
#ifdef DEBUG_MODE
    //int rows = 0,cols = 0;
    //int out_rows = 0,out_cols = 0;
    //manager->getInputImageSize(cols,rows);
   // const double coefs[3][3] = {0.87,0.5,0,-0.5,0.87,0,0,0,1};
   // M.set(coefs);
#endif /*DEBUG_MODE*/

    /*calculate the size of the output warped inage using forward transform*/
    manager->ForwardTransform(M);
   
 #ifdef DEBUG_MODE 
    // printout the final matrix M
    std::cout << "Final Forward Map M = : " << std::endl;
    M.print();
#endif /*DEBUG_MODE*/
    
   //perform perspective warp
   if(manager->warpMode == 0)
   {
		/*calculate the inverse transform*/
		invM = M.inverse();

#ifdef DEBUG_MODE 
		std::cout << "Inverse Map inv(M) = : " << std::endl;
		invM.print();
#endif /*DEBUG_MODE*/
   
		/*Perform inverse transform to get the warped image*/
		manager->InverseTransform(invM);
   }
   else if(manager->warpMode == 1)
   {
	   /*Perform Bilinear transform to get the warped image*/
		manager->BilinearWarp();
   }
   else if (manager->warpMode == 2)
   {
	   double s; double cx; double cy;
	    std::cout<<"Twirl Mode - enter factors :"<<std::endl;
	    std::cout<< "Center factor cx"<<std::endl;
        std::cin>>cx;
        std::cout<< "Center factor cy"<<std::endl;
        std::cin>>cy;
        std::cout<< "strength factor s"<<std::endl;
        std::cin>>s;
		/*Perform twirl transform to get the warped image*/
		manager->TwirlWarp(s, cx, cy);
   }
   
   if(0 != out.length())
   manager->WriteImage();
   
   /*Initialize the GLUT*/
	InitGlut(argc,argv);
   
   	/*display the warped image in window*/
	manager->ReshapeWindow();
	glutPostRedisplay();	
	
	/*GLUT event processing infinite loop*/
	glutMainLoop();

   return 0;
}
