/**********************************************************************
*
*  FILE NAME	: header.h
*
*  DESCRIPTION  : File to include the header files.
* 
*  PLATFORM		: Windows
*
*  DATE	               	NAME	        	  	REASON
*  29th Oct,2017        Shashi Shivaraju        ECE_6310_Lab_05
*                       [C88650674]
***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <math.h>

#define CONTOUR_ITERATIONS 30
#define SQR(x) ((x)*(x))

typedef struct __contour_position
{
	int			 Count;
	int			 y_pos;
	int			 x_pos;
	int			 index;
	struct contour_position *next;
}contour_position;

contour_position* insert_contour_position(contour_position* head,int index,
							int x_pos,int y_pos,contour_position* prev);

void destroy_contourlist(contour_position* head);

void Display_Contour(unsigned char *image,int ROW,int COL,contour_position* contour_head);

int Sobel_Convolution(unsigned char *image,int ROW,int COL);

void ActiveContourAlgo(unsigned char *image,unsigned char *sobel_image,contour_position* contour_head,int ROW,int COL);

void convert_index2height_width(int index,
								int ROW,int COL,/* size of image */
								int *x_pos,int *y_pos);/* pixel position */

void convert_height_width2index(int *index,
								int ROW,int COL,/* size of image */
								int x_pos,int y_pos);/* pixel position */

void CalculateAverageContourDistance(contour_position* contour_head,float *avrg_distance);
