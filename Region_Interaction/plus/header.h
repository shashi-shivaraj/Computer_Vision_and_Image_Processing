#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <process.h>	/* needed for multithreading */


#define SQR(x) ((x)*(x))

#define PLUS_CUSTOM_OVERLAPPEDWINDOW (WS_OVERLAPPED     | \
                             WS_CAPTION        | \
                             WS_SYSMENU        | \
                             WS_THICKFRAME)

typedef enum __PixelColour
{
	eRed,
	eGreen,
	eBlue 
}ePixelColour;

enum
{
	ePlayMode,
	eStepMode
}eDisplayMode;


typedef struct __user_options
{
	int			 Count;
	int			 row;
	int			 col;
	ePixelColour Colour;
	int		IntensityPredicate;
	int		DistancePredicate;
}user_options;

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
				int Intensity_Predicate,
				int Centroid_Predicate);

void convert_index2height_width(int index,
								int ROW,int COL,/* size of image */
								int *x_pos,int *y_pos);/* pixel position */