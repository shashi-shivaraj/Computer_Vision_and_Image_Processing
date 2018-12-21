/**************************************************************************
*  FILE NAME	: rotating_planet.cpp
*
*  DESCRIPTION  : Program to use OpenGL to create and rotate a blue planet.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  2nd Dec,2018        Shashi Shivaraju        CPSC_6780_Assignment_4
*                       [C88650674]
***************************************************************************/
/*Header file inclusions*/
#include <cstdio>
#include <cstring>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

/*global declaration*/
static int rotate = 0.0;

/*Function prototypes*/

/*callback to handle key events*/
void keyboard (unsigned char key, int x, int y);
/*Function to configure the window for display*/
void config_display();
/*display callback function*/
void display();
/*reshape callback function*/
void reshape (int w, int h);
/*Function to load the texture file*/
void load_texture();
/*Function to add blue light to planet*/
void do_lights();
/*Function to load the texture to the planet*/
void do_material();
/*Callback for the timer event*/
void update(int value);


/*Main function of the program*/
int main(int argc,char *argv[])
{	 
   glutInit(&argc, argv); /*initialize GLUT*/
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); /*set display mode*/
   glutInitWindowSize (500, 500);  /*set window size*/
   glutInitWindowPosition (100, 100); /*set window position*/
   glutCreateWindow ("Blue Planet");  /*Create the display window*/
   
   config_display(); /*Set the display to black*/
   
   glutKeyboardFunc(keyboard); /*set the keyboard callback function*/
   glutReshapeFunc(reshape); /*set the reshape callback*/
   glutDisplayFunc(display); /*set the display callback function to display the planet*/
   load_texture(); /* load the texture file for the planet from the PPM file*/
   do_lights(); /*add blue light*/
   do_material(); /*add the texture materials*/
   glutTimerFunc(25, update, 0);/*set the timer function to rotate the planet*/
   glutMainLoop(); /*GLUT event loop*/
   
   return 0;
}

/*keyboard callback function*/
void keyboard (unsigned char key, int x, int y)
{
   switch (key) {
	   /*close the display window on q or esc keys*/
		case 'q':
		case 'Q':
	    case 27:
		{
			exit(0);
		}
		break;
        default:
        break;
   }
}

/*display callback function*/
void display()
{
   glClear (GL_COLOR_BUFFER_BIT);/*clear the screen*/
   glPushMatrix();
   GLUquadric *quad = gluNewQuadric(); /*planet by creating a quadrics object*/
   gluQuadricTexture(quad, 1); /*texturing is desired*/
   glEnable(GL_TEXTURE_2D); /*enable texture rendering*/
   glRotatef(rotate,0.0,1.0,0.0); /*rotate on y aixs*/
   gluSphere(quad, 5, 20, 20); /*radius=5, slices=20, and stacks=20, centered around the origin*/
   glPopMatrix();
   glutSwapBuffers();
}

/*callback for reshape event */
void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 0.1, 20);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt (0.0, 0.0, 20.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

/*Callback for the timer event*/
void update(int value)
{
	rotate+=2.0f; /*increase the rotation angle*/
	if(rotate>360.f)
	{	
		rotate-=360;
	}
	glutPostRedisplay(); /*trigger display callback*/
	glutTimerFunc(25,update,0); /*update the timer*/
}

/*config the window for display*/
void config_display() 
{
   glClearColor (0.0, 0.0, 0.0, 0.0); /*set black background*/
   glShadeModel (GL_SMOOTH);/*set smooth shading*/
}

/*Function to load the texture file*/
void load_texture()
{
FILE *fptr;
char buf[512];
int im_size, im_width, im_height, max_color;
unsigned char *texture_bytes; 
char *parse;

// Load a ppm file and hand it off to the graphics card.
fptr=fopen("scuff.ppm","r");
fgets(buf,512,fptr);
do{
	fgets(buf,512,fptr);
	} while(buf[0]=='#');
parse = strtok(buf," ");
im_width = atoi(parse);

parse = strtok(NULL,"\n");
im_height = atoi(parse);

fgets(buf,512,fptr);
parse = strtok(buf," ");
max_color = atoi(parse);

im_size = im_width*im_height;
texture_bytes = (unsigned char *)calloc(3,im_size);
fread(texture_bytes,1,3*im_size,fptr);
fclose(fptr);

glBindTexture(GL_TEXTURE_2D,1);
glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,im_width,im_height,0,GL_RGB, 
	GL_UNSIGNED_BYTE,texture_bytes);
glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
// We don't need this copy; the graphics card has its own now.
free(texture_bytes);
}

/*Function to add blue light*/
void do_lights()
{
float light0_ambient[] = { 0.0, 0.0, 1.0, 1.0 };
float light0_diffuse[] = { 0.0, 0.0, 1.0, 1.0 };
float light0_specular[] = { 1.0, 1.0, 10.0, 5.0 }; /*set specular light to highlight the center area of the surface*/
float light0_position[] = { 0.0, 0, 1, 0.0 };
float light0_direction[] = { 1.0, 1.0, 1.0, 1.0};
float mat_shininess[] = {5.0};

// Turn off scene default ambient. 
glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light0_ambient);

// Make specular correct for spots.
glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1);

glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient);
glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse);
glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular);
glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);

glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,200);
glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,120.0);
glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light0_direction);

glEnable(GL_LIGHTING);
glEnable(GL_LIGHT0);

}

/*Function to load the texture to the planet*/
void do_material()
{
float mat_ambient[] = {0.0,0.0,1.1,10.0};
float mat_diffuse[] = {0.8,0.8,0.8,5.0};
float mat_specular[] = {0.5,0.5,0.5,20.0};
float mat_shininess[] = {2.0};
glMaterialfv(GL_FRONT,GL_AMBIENT,mat_ambient);
glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
}

