
#define SQR(x) ((x)*(x))	/* macro for square */
#ifndef M_PI			/* in case M_PI not found in math.h */
#define M_PI 3.1415927
#endif
#ifndef M_E
#define M_E 2.718282
#endif

#define MAX_FILENAME_CHARS	320
#define DEFAULT_INTENSITY_PREDICATE 15
#define DEFAULT_CENTROID_DISTANCE_PREDICATE 500

char	filename[MAX_FILENAME_CHARS];

HWND	MainWnd;
HANDLE  ghMutex;
contour_position *list_head[100] = {0};
contour_position	*new_data = NULL,*prev_data = NULL;
contour_position	*alter_contour_node = NULL;

FILE			 *Gfp = NULL;			

		// Display flags
int		ShowPixelCoords = 0;/*default disabled*/
int		InitialContourDraw = 0;
int		AlterContour	   = 0;
int		AlterContourThread = 0;
int		AlterContour_xPos = 0,AlterContour_yPos = 0;
int		PixelColour = 0;/*default red*/
int		DisplayMode = 0;/*default play*/
int		IntensityPredicate = DEFAULT_INTENSITY_PREDICATE;/*default value = 15*/
int		DistancePredicate = DEFAULT_CENTROID_DISTANCE_PREDICATE; /*default vlaue = 500*/
int		thread_count = 0;

int		screen_width = 0;
int		screen_height = 0;

// Reset Flag

		// Image data
unsigned char	*FileImage = NULL;/*Image loaded from the file (either P5/P6)*/
unsigned char	*OriginalImage = NULL; /*P5 type PPM image used for display and processing */
unsigned char	*sobel_image = NULL; /*pointer to store convolution of image data with sobel filter*/
unsigned char	*labels = NULL;
int				TotalRegions = 0;
int				contour_count		 = 0;


Algo_options Algo_Opt = {0};
//
//#define LEFT_CLICK_WINDOW 3
//#define RIGHT_CLICK_WINDOW 7
//#define ALTER_CLICK_WINDOW 5

BOOL			ThreadRun[255];/*total threads = total regions possible = 255*/
BOOL			StepModeState[255];

int				ROWS,COLS;

#define TIMER_SECOND	1			/* ID of timer used for animation */

		// Drawing flags
int		TimerRow,TimerCol;
int		ThreadRow,ThreadCol;
int		ThreadRunning;

		// Function prototypes
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
void PaintImage();
void AnimationThread(void *);		/* passes address of window */
void RegionGrowThread(void* aoptions); /*passs address of the stucture with user selected options*/