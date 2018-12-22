/**********************************************************************
*
*  FILE NAME	: 7x7_mean_filter.c
*
*  DESCRIPTION  : Implementation of 7x7 mean filter using basic 2D 
*                 convolution,seperable filters and sliding window 
*                 computations to achieve smoothing of the image.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  5th Sep,2017         Shashi Shivaraju        ECE_6310_Lab_01
*                       [C88650674]
***********************************************************************/
 
/*Header file inclusions*/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>	/*included for strncmp() and memset() api*/
#include <time.h>	/*included for clock_gettime() api*/

/*Macro Declarations*/

#define TIME_PROFILE_ENABLED

#undef DEBUG_MODE	/*Enable this debug macro to dump pixel values on console*/
                    /*Do Not enable DEBUG_MODE when time profiling is being done */

#ifdef DEBUG_MODE
#define	DEBUG_HEIGHT 12;	/*number of rows*/
#define DEBUG_WIDTH  16; 	/*number of columns*/
#endif /*DEBUG_MODE*/


int main(int argc,char* argv[])/*main function of the program*/
{
	
	FILE* fp = NULL;                /*File pointer for file operations*/	
	
	unsigned char *image = NULL;	/*pointer to store input image data*/
	unsigned char *smooth = NULL; 	/*pointer to store output image data*/
	int *intermediate = NULL;       /*pointer to store intermediate data obtained during seperable filter convolution */
	char magic[10];                 /*array to store header of the input image*/
	
#ifdef TIME_PROFILE_ENABLED
	struct timespec	tp1,tp2;        /*variables to store time*/
#endif /*TIME_PROFILE_ENABLED*/
	int row=0;                      /*variable to store height of input image*/
	int col=0;                      /*variable to store width of input image*/
	int max_pixel=0;                /*maximum pixel value of the image*/
	int ret=0;                      /*variable to store return values of API*/
	int R=0,C=0,r1=0,c1=0,sum=0,i=0;	/*variables used in convolution computation loop*/
	
	int N0 = 7;						/*variable which stores kernel order for 7x7,N0=7*/
	int N = N0/2;					/*variable which stores kernel order by 2 i.e for 7x7,N=3 & 3X3,N=1*/
		
	if(2 != argc)                   /*error handling*/
	{
		printf("usage is ./[exe] [file] \n");/*display proper usage for execution*/
		return -1;              /*return error code*/
	}
			
	fp = fopen(argv[1],"r");        /*open input image file provided as cmd line arg*/
	if(!fp)                         /*error handling*/
	{
		printf("fopen failed for %s\n",argv[1]);/*failure to open the input file*/
		return -1;              /*return error code*/	
	}
	
	ret = fscanf(fp,"%s %d %d %d "
	,magic,&col,&row,&max_pixel);	/*read header information of the image*/
	if(	4 != ret || 
		255 != max_pixel ||
		0 != strcmp("P5",magic))/*error handling specific to 8bit greyscale PPM image*/
	{
		printf("Not a greyscale image of PPM format\n");/*Not 8bit greyscale PPM image */
		return -1;              /*return error code*/	
	}
	
#ifdef DEBUG_MODE	
	row = DEBUG_HEIGHT;             /*hardcoding the image size for debug analysis*/
	col = DEBUG_WIDTH;		
#endif /*DEBUG_MODE*/
	
	printf("Header info of the opened PPM file is max pixel = %d,\
col = %d,row = %d, magic code = %s\n",max_pixel,col,row,magic);/*Display header info of image*/

	image = (unsigned char *)calloc(row*col,sizeof(unsigned char));/*Allocate memory to store input image data*/
	if(!image)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
	}
	
	smooth = (unsigned char *)calloc(row*col,sizeof(unsigned char));/*Allocate memory to store output image data*/
	if(!smooth)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;		/*return error code*/
	}
	
	intermediate = (int *)calloc(row*col,sizeof(int));/*Allocate memory to store intermediate data after 1D convolution*/
	if(!intermediate)		/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
	}
	
	ret = fread(image,1,row*col,fp);/*read the image data form file and store in a buffer*/
	if(ret != row*col)              /*check for invalid fread*/
	{
		printf("fread failed to read %d data from file",row*col); /*fread operation failed*/
		return -1;              /*return error code*/
	}

	if(fp)                          /*close the file handle of  input file*/
	{
		fclose(fp);
		fp = NULL;
	}

#ifdef DEBUG_MODE
	printf("Original image is :\n");
	for(i = 0;i<row*col;i++)
	{
		if(!(i%col) && i!=0)
		printf("\n");
		printf("%d\t",image[i]);
	}
	printf("\n");
#endif /*DEBUG_MODE*/

#ifdef TIME_PROFILE_ENABLED
clock_gettime(CLOCK_REALTIME,&tp1);/* query timer before 2D convolution */
printf("[Before][2D Convolution]Time in sec %ld ns %ld\n",
(long int)tp1.tv_sec,tp1.tv_nsec);/*display before timer value*/
#endif /*TIME_PROFILE_ENABLED*/
	 
/*2D convolution of 7x7 mean filter with 
 * the input image excluding the borders*/

	 for(R=N;R<row-N;R++)
	 {
		 for(C=N;C<col-N;C++)
		 {
			 sum = 0;
			 for(r1=-N;r1<=N;r1++)
			 {
				 for(c1=-N;c1<=N;c1++)
				 {
					sum = sum +image[(R+r1)*col+(C+c1)];
				 }
			 }
			 smooth[col*R+C]=sum/(N0*N0);
		 }
	 }
	 
#ifdef TIME_PROFILE_ENABLED
clock_gettime(CLOCK_REALTIME,&tp2);/* query timer after 2D convolution*/
printf("[After][2D Convolution]Time is sec %ld ns %ld\n",
(long int)tp2.tv_sec,tp2.tv_nsec);/*display after timer value */
printf("Time taken for 2D Convolution = %ld\n\n",tp2.tv_nsec-tp1.tv_nsec);/*display the time taken for 2D convolution*/
#endif /*TIME_PROFILE_ENABLED*/
	
	fp = fopen("smooth_2d_conv.ppm","w+");/*open output image file*/
	if(!fp)                     /*error handling*/
	{
		printf("fopen failed for %s\n","smooth_2d_conv.ppm");/*failure to open the output file*/
		return -1;                  /*return error code*/
	}
	
	fprintf(fp,"P5 %d %d 255 ",col,row);/*Write the header as per PPM image specification to output image file*/
	fwrite(smooth,1,row*col,fp);/*write the output image data into file*/
	
	if(fp)                      /*Close the output file handle*/
	{
		fclose(fp);
		fp = NULL;
        }

#ifdef DEBUG_MODE
	printf("2D Conv image is :\n");
	for(i = 0;i<row*col;i++)
	{
		if(!(i%col) && i!=0)
		printf("\n");
		printf("%d\t",smooth[i]);
	}
	printf("\n");
#endif /*DEBUG_MODE*/
	
	memset(smooth,0,row*col*sizeof(unsigned char)); /*initialize the char array to zero*/

#ifdef TIME_PROFILE_ENABLED	
clock_gettime(CLOCK_REALTIME,&tp1);   /*query timer before separable convolution*/
printf("[Before][Seperable Filter]Time in sec %ld ns %ld\n",
        (long int)tp1.tv_sec,tp1.tv_nsec); /*display before timer value*/
#endif /*TIME_PROFILE_ENABLED*/

/* Convolution of 7x7 mean filter with the input image 
 * excluding the borders using seperable filters */
	
	 for(C=0;C<col;C++)                   /*1D convolution with row seperable filter*/
	{
		for(R=0;R<row;R++)
		{
			 sum = 0;
			 for(r1=0;r1<=2*N;r1++)
			 { 
				 if((R+r1)*col + C < row*col)			/*check added to prevent invalid read*/
				 sum = sum +image[(R+r1)*col + C];
			 }
			 intermediate[R*col+C]=sum;
		}
	}
		
	for(R=0;R<row-(2*N);R++)               /*1D convolution with column seperable filter*/
	{
		 for(C=N;C<col-N;C++)
		 {
			 sum = 0;
			 for(c1=-N;c1<=N;c1++)
			 {
				sum = sum +intermediate[(R*col)+(C+c1)];
			 }
			 smooth[col*(R+N)+C]=(unsigned char)(sum/(N0*N0));
		 }
	 }

#ifdef TIME_PROFILE_ENABLED
clock_gettime(CLOCK_REALTIME,&tp2); /* query timer after seperable convolution*/
printf("[After][Seperable Filter]Time is sec %ld ns %ld\n"
        ,(long int)tp2.tv_sec,tp2.tv_nsec);/*display timer after seperable convolution*/
printf("Time taken for convolution with Seperable Filters = %ld\n\n"/* report how long it took to smooth */
        ,tp2.tv_nsec-tp1.tv_nsec);
#endif /*TIME_PROFILE_ENABLED*/

	fp = fopen("smooth_sep_filter_conv.ppm","w+");/*open output image file*/
	if(!fp)                     /*error handling*/
	{
		printf("fopen failed for %s\n","smooth_sep_conv.ppm");/*failure to open the output file*/
		return -1;                   /*return error code*/
	}
	
	fprintf(fp,"P5 %d %d 255 ",col,row);/*Write the header as per PPM image specification to output image file*/
	fwrite(smooth,1,row*col,fp);/*Write output image data into file*/

        if(fp)                      /*Close output file handle*/
	{
		fclose(fp);
		fp = NULL;
	}
	
#ifdef DEBUG_MODE
	printf(" 1D Conv intermediate image for row seperable filter is :\n");
	for(i = 0;i<row*col;i++)
	{
		if(!(i%col) && i!=0)
		printf("\n");
		printf("%d\t",intermediate[i]);
	}
	printf("\n");
	
	printf(" Convolved final image using seperable filters is :\n");
	for(i = 0;i<row*col;i++)
	{
		if(!(i%col) && i!=0)
		printf("\n");
		printf("%d\t",smooth[i]);
	}
	printf("\n");
#endif /*DEBUG_MODE*/

/* Convolution of 7x7 mean filter with the input image 
 * excluding the borders using seperable filters and sliding window */

    memset(smooth,0,row*col*sizeof(unsigned char)); /*initialize the char array to zero*/
    
    for(i=0;i<row*col;i++)
        intermediate[i] = 0;                        /*initialize the integer array to zero*/

#ifdef TIME_PROFILE_ENABLED
clock_gettime(CLOCK_REALTIME,&tp1);   /*query timer before separable convolution*/
printf("[Before][Separable Filter + Sliding Window]Time in sec %ld ns %ld\n",
(long int)tp1.tv_sec,tp1.tv_nsec); /*display before timer value*/
#endif /*TIME_PROFILE_ENABLED*/
                		
        for(C=0;C<col;C++)                   /*1D convolution with row seperable filter and sliding window*/
	{
		for(R=0;R<row;R++)
		{			
			 if(0 == R)
			 {
				 sum = 0;
				 for(r1=0;r1<=2*N;r1++)
				 { 
					   sum = sum +image[(R+r1)*col + C];
				 }
			 }
			 else
			 {
				 if(((R-1)*col+ C) < row*col && ((R+2*N)*col+C) < row*col) /*check added to prevent invalid read*/
				 sum = sum-image[(R-1)*col+ C]+image[(R+2*N)*col+C];
			 }
			
			 intermediate[R*col+C]=sum;
		}
	}
	
	for(R=0;R<row-(2*N);R++)                 /*1D convolution with column seperable filter and sliding window*/
	{
		 for(C=0;C<col-(2*N);C++)
		 {
			 if(0 == C)
			 {
				 sum = 0;
				 for(c1=0;c1<=2*N;c1++)
				 {
					 sum = sum +intermediate[(R*col)+c1];
				 }
			 }
			 else
			 {
				 sum = sum - intermediate[(R*col)+(C-1)]+intermediate[(R*col)+(C+2*N)];
			 }
			 smooth[col*(R+N)+(C+N)]=(unsigned char)(sum/(N0*N0));
		 }
	 }

#ifdef TIME_PROFILE_ENABLED
clock_gettime(CLOCK_REALTIME,&tp2); /* query timer after seperable convolution*/
printf("[After][Seperable Filter+Sliding Window]Time is sec %ld ns %ld\n"
        ,(long int)tp2.tv_sec,tp2.tv_nsec);/*display timer after seperable convolution*/
printf("Time taken for convolution with Seperable Filters+Sliding Window = %ld\n\n"/* report how long it took to smooth */
        ,tp2.tv_nsec-tp1.tv_nsec);
#endif /*TIME_PROFILE_ENABLED*/
 
	fp = fopen("sep_filter_sliding_win_conv.ppm","w+");/*open output image file*/
	if(!fp)                     /*error handling*/
	{
		printf("fopen failed for %s\n","sep_filter_sliding_win_conv.ppm");/*failure to open the output file*/
		return -1;                   /*return error code*/
	}
	
	fprintf(fp,"P5 %d %d 255 ",col,row);/*Write the header as per PPM image specification to output image file*/
	fwrite(smooth,1,row*col,fp);/*Write output image data into file*/

        if(fp)                      /*Close output file handle*/
	{
		fclose(fp);
		fp = NULL;
	}
	 

#ifdef DEBUG_MODE
	printf(" 1D Conv intermediate image with row seperable filter and sliding window is :\n");
	for(i = 0;i<row*col;i++)
	{
		if(!(i%col) && i!=0)
		printf("\n");
		printf("%d\t",intermediate[i]);
	}
	printf("\n");

	printf(" Convolved final image using seperable filters and sliding window is :\n");
	for(i = 0;i<row*col;i++)
	{
		if(!(i%col) && i!=0)
		printf("\n");
		printf("%d\t",smooth[i]);
	}
	printf("\n");
#endif /*DEBUG_MODE*/
 
 
/********************************Resource Deallocations********************************************/
	if(image)                           /*free the allocated memory for input image*/  
	{
		free(image);
		image = NULL;
	}
	
	if(smooth)                          /*free the allocated memory for output image*/
	{
		free(smooth);
		smooth = NULL;
	}

	if(intermediate)                    /*free the allocated memory for intermediate result*/
        {
		free(intermediate);
		intermediate = NULL;
	}
	
	return 0;                           /*return success code zero*/
}
