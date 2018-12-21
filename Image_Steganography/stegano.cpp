/**************************************************************************
*  FILE NAME	: stegno.cpp
*
*  DESCRIPTION  : Program to perform image steganography.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  20th Nov,2018        Shashi Shivaraju        CPSC_6040_Final_Project
*                       [C88650674]
***************************************************************************/
/*Header file  inclusion*/
#include "manager.h"

/*Have used a singleton design pattern to remove use of global variables*/
Manager* Manager::instance = NULL;

/*main function of the program*/
int main(int argc,char *argv[])
{
	/*variable declarations*/
	int ret = 0;
	usagemode usage_mode = e_hide;
	steganomode stegano_mode = e_default;
	std::string cover,secret,hidden;
	
	/*check for valid command line args*/
	if(argc != 6)
	{
		std::cout<<"[Usage to Hide image:]./stegano h mode cover.img secret.img hidden.img"<<std::endl;
		std::cout<<"mode = d for default steganography"<<std::endl;
		std::cout<<"mode = c for compression based steganography "<<std::endl;
		
		std::cout<<"[Usage to Unhide image:]./stegano uh mode hidden.img cover.img secret.img"<<std::endl;
		std::cout<<"mode = d for default steganography"<<std::endl;
		std::cout<<"mode = c for compression based steganography "<<std::endl;
		
		return -1;
	}
	
	/*set the mode for program usage*/
	if(!strcmp(argv[1],"h"))
	{
		usage_mode = e_hide;
		std::cout<<"usage mode = hide "<<std::endl;
		
		cover = argv[3];
		secret = argv[4];
		hidden = argv[5];
	}
	else if(!strcmp(argv[1],"uh"))
	{
		usage_mode = e_unhide;
		std::cout<<"usage mode = unhide "<<std::endl;
		
		hidden = argv[3];
		cover = argv[4];
		secret = argv[5];
		
	}
	else
	{
		std::cout<<"invalid usgae mode; use h or uh"<<std::endl;
		return -1;
	}
	
	/*set the mode for steganography usage*/
	if(!strcmp(argv[2],"d"))
	{
		stegano_mode = e_default;
		std::cout<<"steganography mode = default "<<std::endl;
	}
	else if(!strcmp(argv[2],"c"))
	{
		stegano_mode = e_compressed;
		std::cout<<"steganography mode = compression "<<std::endl;
	}
	else
	{
		std::cout<<"invalid usgae mode; use d or c"<<std::endl;
		return -1;
	}
	
	/*Create a Manager*/
	Manager* manager = Manager::getInstance();	
	/*Set file names*/
	manager->setFilenames(cover,secret,hidden);
	/*Set usage mode*/
	manager->setUsage(usage_mode);
	/*Set stegano mode*/
	manager->setMode(stegano_mode);
	
	/*Perform steganography*/
	ret = manager->PerformSteganography();
	if(ret != 0)
	{
		std::cout<<"PerformSteganography failed  with "<< ret <<std::endl;
		//return ret; /*fall through to allow memory deallocation when user presses esc/q*/
	}
	
	/*Initialize the GLUT*/
	InitGlut(argc,argv);
	
   	/*Display the result in window*/
	manager->ReshapeWindow();
	glutPostRedisplay();	
	
	/*GLUT event processing infinite loop*/
	glutMainLoop();
	
	return ret;
}
