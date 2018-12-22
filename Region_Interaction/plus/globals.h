
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

		// Display flags
int		ShowPixelCoords = 0;/*default disabled*/
int		PixelColour = 0;/*default red*/
int		DisplayMode = 0;/*default play*/
int		IntensityPredicate = DEFAULT_INTENSITY_PREDICATE;/*default value = 15*/
int		DistancePredicate = DEFAULT_CENTROID_DISTANCE_PREDICATE; /*default vlaue = 500*/

// Reset Flag

		// Image data
unsigned char	*OriginalImage;
unsigned char	*labels;
int				TotalRegions = 0;
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
void RegionGrowThread(void* aoptions); /*pases address of the stucture with user selected options*/