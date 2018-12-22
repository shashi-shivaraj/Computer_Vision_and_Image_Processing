#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <process.h>	/* needed for multithreading */

#undef DEBUG_MODE
#define SQR(x) ((x)*(x))
#define CONV_2_DEGREE(x) (x*180)/3.14159265358979323846 
#define CONV_2_RADIAN(x)  (x*3.14159265358979323846)/180
//#define	LEFT_CONTOUR_ITERATIONS 40
//#define RIGHT_CONTOUR_ITERATIONS 50 
//#define ALTER_CONTOUR_ITERATIONS 15 
//
//
//#define LEFT_CLICK_WINDOW 3
//#define RIGHT_CLICK_WINDOW 7
//#define ALTER_CLICK_WINDOW 5

#define PLUS_CUSTOM_OVERLAPPEDWINDOW (WS_OVERLAPPED     | \
	WS_CAPTION        | \
	WS_SYSMENU        | \
	WS_THICKFRAME)

enum
{
	eMovable,
	eFixed
}ePosition;


typedef struct __contour_position
{
	int			 Thread_index;
	int			 Count;
	int			 y_pos;
	int			 x_pos;
	int			 last_y_pos;
	int			 last_x_pos;
	int			 index;
	int			 pos_state;
	struct contour_position *next;
	struct contour_position *prev;
}contour_position;

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


enum
{
	eP5,
	eP6
}eImageType;

enum
{
	eErase,
	eDisplay
}eVisual;


typedef struct __user_options
{
	int			 Count;
	int			 row;
	int			 col;
	ePixelColour Colour;
	int		IntensityPredicate;
	int		DistancePredicate;
}user_options;

typedef struct __Algo_options
{
	int left_contour_iteration;
	int left_click_window;
	int left_IE1_W;
	int left_IE2_W;
	int left_EE1_W;
	int left_EE2_W;

	int right_contour_iteration;
	int right_click_window ;
	int right_IE1_W;
	int right_IE2_W;
	int right_EE1_W;
	int right_EE2_W;
	int right_EE3_W;

	int alter_contour_iteration;
	int alter_click_window;
	int alter_IE1_W;
	int alter_IE2_W;
	int alter_EE1_W;
	int alter_EE2_W;

}Algo_options;


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

void convert_rgb_to_grey(unsigned char *image,unsigned char *grey_image,int ROW,int COL);
void downsample_rgb_image(unsigned char *image,unsigned char *output,int ROW,int COL);
void downsample_grey_image(unsigned char *image,unsigned char *output,int ROW,int COL);
int Sobel_Convolution(unsigned char *image,int ROW,int COL);

void LeftClickThread(void* aoptions); /*pases address of the stucture with pointer to initial counter*/
void RightClickThread(void* aoptions); /*pases address of the stucture with pointer to clicked point*/
void AlterActiveContourThread(void* aoptions);

void CalculateAverageContourDistance(contour_position* contour_head,float *avrg_distance);

void CalculateAverageContourRGBIntensity(contour_position* contour_head,unsigned char *rgb_image,int ROWS,int COLS,
										 float *avrg_red,float *avrg_green,float *avrg_blue);
void CalculateContourCentroid(contour_position* contour_head,float *centroid_xpos,float *centroid_ypos);

void RubberBandActiveContourAlgo(unsigned char *image,
								 unsigned char *sobel_image,
								 unsigned char *rgb_image,
								 contour_position* contour_head,
								 int ROW,int COL,
								 Algo_options* AlgoOpt);

void BalloonActiveContourAlgo(unsigned char *image,
							  unsigned char *sobel_image,
							  unsigned char *rgb_image,
							  contour_position* contour_head,
							  int ROW,int COL,
							  int seed_xpos,int seed_ypos,
							  Algo_options* AlgoOpt);

void AlterActiveContourAlgo(unsigned char *image,
							unsigned char *sobel_image,
							unsigned char *rgb_image,
							contour_position* contour_head,
							int ROW,int COL,
							Algo_options* AlgoOpt);


int convert_index2height_width(int index,
							   int ROW,int COL,/* size of image */
							   int *x_pos,int *y_pos);/* pixel position */

int convert_height_width2index(int *index,
							   int ROW,int COL,/* size of image */
							   int x_pos,int y_pos);/* pixel position */

int convert_height_width2rgbindex(int *index,
								  int ROW,int COL,/* size of image */
								  int x_pos,int y_pos);/* pixel position */

int MutexLock(void* h_Mutex);
int MutexUnlock(void* h_Mutex);

void update_contour_display(contour_position* contour_head,int ROW,int COL,unsigned char *labels,int option);
