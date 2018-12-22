#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>


#define DEBUG_MODE
#define SQR(x) ((x)*(x))
#define INTENSITY_THRESHOLD	126
#define MIN_REGION_SIZE		10
#define NORM_DIST			3
#define	E_TRUE				1
#define E_FALSE				0
#define GREY_SHADE			40
#define	ORIENTATION_THRESHOLD 40 /*(Degrees)*/
#define CONV_2_DEGREE(x) (x*180)/3.14159265358979323846 
#define CONV_2_RADIAN(x)  (x*3.14159265358979323846)/180

/* function prototypes */

/*
** Given an image, a starting point, and a label, this routine
** paint-fills (8-connected) the area with the given new label
** according to the given criteria (pixels close to the average
** intensity of the growing region are allowed to join).
*/
void RegionGrow(unsigned char *image,	/* image data */
				unsigned char *labels,	/* segmentation labels */
				int ROWS,int COLS,	/* size of image */
				int r,int c,		/* pixel to paint from */
				int paint_over_label,	/* image label to paint over */
				int new_label,		/* image label for painting */
				int *indices,		/* output:  indices of pixels painted */
				int *count,			/* output:  count of pixels painted */
				double **norms,	/*3D norms of the image*/
				int Orientation_Predicate);

int convert_index2height_width(int index,
								int ROW,int COL,/* size of image */
								int *x_pos,int *y_pos);/* pixel position */

int convert_height_width2index(int *index,
								int ROW,int COL,/* size of image */
								int x_pos,int y_pos);/* pixel position */

void odetics2coords(unsigned char *RangeImage,int ROWS,int COLS,double **P,int ImageTypeFlag);