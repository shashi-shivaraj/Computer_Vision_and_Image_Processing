/**************************************************************************
*  FILE NAME	: convolution.cpp
*
*  DESCRIPTION  : Program to perform convolution of a filter with a 
* 				  image and to display the image.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  12th Oct,2018        Shashi Shivaraju        CPSC_6040_HW_04
*                       [C88650674]
***************************************************************************/
/*Header file  inclusion*/
#include "image.h"

/*Have used a singleton design pattern to remove use of global variables*/
Manager* Manager::instance = NULL;

int main(int argc,char*argv[])
{
	int ret = 0;	/*varialbe to store return value*/
	std::string input,filter,out;
	float theta = 0,sigma = 0,period = 0;
	/*check for valid command line args*/
	switch (argc)
	{
		case 3:/*[exe] [input.png] [filter.filt]*/
		{
			input = argv[1];
			filter = argv[2];
		}
		break;
		case 4: /*[exe] [input.png] [filter.filt] [out.png]*/
		{
			input = argv[1];
			filter = argv[2];	
			/*write the currently displayed image to file*/
			out = argv[3];
		}
		break; 
		case 6: /*[exe] [input.png] [-g] [theta] [sigma] [period][optional out.png]*/
		{
			input = argv[1];
			if (0 == strcmp(argv[2],"-g"))
			{
				theta = atof(argv[3]);
				sigma = atof(argv[4]);
				period = atof(argv[5]);
				
				if(period == 0 || sigma == 0)
				{
					std::cout<<"period/sigma cannot be 0"<<std::endl;
					return -1;
				}
			}
		}
		break;
		case 7:/*[exe] [input.png] [-g] [theta] [sigma] [period][out.png]*/
		{
			input = argv[1];
			if (0 == strcmp(argv[2],"-g"))
			{
				theta = atof(argv[3]);
				sigma = atof(argv[4]);
				period = atof(argv[5]);
				
				if(period == 0 || sigma == 0)
				{
					std::cout<<"period/sigma cannot be 0"<<std::endl;
					return -1;
				}
			}
			out = argv[6];
		}
		break;
		default:
		{
			std::cout<<"usage:./[exe] [input.png] [filter.filt] [optional out.png]"<<std::endl;
			std::cout<<"or usage:./[exe] [input.png] [-g] [theta] [sigma] [period][optional out.png]"<<std::endl;
			return -1;
		}
	}
	
	/*Create a Manager*/
	Manager* manager = Manager::getInstance();
	/*Set file names*/
	manager->setFilenames(input,out,filter);
	
	/*Initialize the GLUT*/
	InitGlut(argc,argv);
	
	/*Read the input image and display it*/
	ret = manager->ReadImage(input);
	if(ret != 0)
	{
		std::cout<<"Read Image failed"<<std::endl;
		return ret;
	}
	
	/*Construct the Kernel*/
	if (0 == strcmp(argv[2],"-g"))
	{
		/*set the Gabor  specs*/
		manager->setGaborSpecs(theta,sigma,period);
		/*build the Gabor kernel*/
		manager->buildGaborKernel();
	}
	else
	{
		/*Read the filter file*/
		ret = manager->ReadFilter();
		if(ret != 0)
		{
			std::cout<<"Read Filter failed"<<std::endl;
			return ret;
		}
	}
	
	/*display the composition image in window*/
	manager->ReshapeWindow();
	glutPostRedisplay();	
	
	/*GLUT event processing infinite loop*/
	glutMainLoop();
	return 0;
}
