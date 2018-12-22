/**********************************************************************
*
*  FILE NAME	: active_contour.c
*
*  DESCRIPTION  : Implementation of active contour algorithm.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  29th Oct,2017        Shashi Shivaraju        ECE_6310_Lab_05
*                       [C88650674]
***********************************************************************/
#include "header.h"


int main(int argc,char* argv[])/*main function of the program*/
{

	FILE* fp = NULL;                /*File pointer for file operations*/	
	unsigned char *image = NULL;	/*pointer to store input image data*/
	unsigned char *initial_image = NULL; /*pointer to store input image data*/
	unsigned char *sobel_image = NULL; /*pointer to store convolution of image data with sobel filter*/
	contour_position *contour_head = NULL,*new_contour = NULL,*prev = NULL,*curr = NULL;
	char magic[10];                 /*array to store header of the input image*/

	int ROW=0;                      /*variable to store height of input image*/
	int COL=0;                      /*variable to store width of input image*/
	int max_pixel=0;                /*maximum pixel value of the image*/
	int ret=0;                      /*variable to store return values of API*/
	int cont_row =0,cont_col =0;

	int index = 0,count = 0,x_pos = 0,y_pos = 0;

	if(3 != argc)                   /*error handling*/
	{
		printf("usage is ./[exe] [image] [contour] \n");/*display proper usage for execution*/
		return -1;              /*return error code*/
	}

	fp = fopen(argv[1],"rb");        /*open input image file provided as cmd line arg*/
	if(!fp)                         /*error handling*/
	{
		printf("fopen failed for %s\n",argv[1]);/*failure to open the input file*/
		return -1;              /*return error code*/	
	}

	ret = fscanf(fp,"%s %d %d %d "
	,magic,&COL,&ROW,&max_pixel);	/*read header information of the image*/
	if(	4 != ret || 
		255 != max_pixel ||
		0 != strcmp("P5",magic))/*error handling specific to 8bit greyscale PPM image*/
	{
		printf("Not a greyscale image of PPM format\n");/*Not 8bit greyscale PPM image */
		return -1;              /*return error code*/	
	}
		
	printf("Header info of the opened PPM file is max pixel = %d,\
col = %d,row = %d, magic code = %s\n",max_pixel,COL,ROW,magic);/*Display header info of image*/

	image = (unsigned char *)calloc(ROW*COL,sizeof(unsigned char));/*Allocate memory to store input image data*/
	if(!image)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
	}

	initial_image = (unsigned char *)calloc(ROW*COL,sizeof(unsigned char));/*Allocate memory to store input image data*/
	if(!initial_image)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
	}

	sobel_image = (unsigned char *)calloc(ROW*COL,sizeof(unsigned char));/*Allocate memory to store input image data*/
	if(!sobel_image)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
	}

	ret = fread(image,1,ROW*COL,fp);/*read the image data form file and store in a buffer*/
	if(ret != ROW*COL)              /*check for invalid fread*/
	{
		printf("fread failed to read %d data from file",ROW*COL); /*fread operation failed*/
		return -1;              /*return error code*/
	}
	
	memcpy((void*)initial_image,(void*)image,ROW*COL);
	memcpy((void*)sobel_image,(void*)image,ROW*COL);

	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}

	/*Read the locations of the intial countour points*/    
    fp = fopen(argv[2],"r");        /*open input ground truth file provided as cmd line arg*/
    if(!fp)                         /*error handling*/
    {
	 printf("fopen failed for %s\n",argv[1]);/*failure to open the input file*/
	 return -1;              /*return error code*/	
    }


	printf("Initial Contours are:");
	count = 0;
	while(0<fscanf(fp,"%d %d ",&cont_col,&cont_row))/*will exit at EOF or fscanf error*/
	{
		printf("Contour Col = %d Contour Row = %d\n",cont_col,cont_row);
		count ++;
		convert_height_width2index(&index,ROW,COL,
								cont_col,cont_row);

		/*Storing the initial contour positions in an singly linked list*/
		new_contour = insert_contour_position(contour_head,index,cont_col,cont_row,prev);
		if(!ret)
		{
			printf("insert_contour_position failed with %d",ret);
		}

		if(!contour_head)
		{
			contour_head = new_contour;
		}

		if(prev)
		prev->next = new_contour;

		new_contour->next = NULL;
		prev = new_contour;
	}
	
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}

	/*To display initail contour position on top of the image using plus marks*/
	Display_Contour(initial_image,ROW,COL,contour_head);

	fp = fopen("intial_contour.ppm","wb+");/*open output image file*/
	if(!fp)                     /*error handling*/
	{
		printf("fopen failed for %s\n","intial_contours.ppm");/*failure to open the output file*/
		return -1;          /*return error code*/
	}
	
	fprintf(fp,"P5 %d %d 255 ",COL,ROW);/*Write the header as per PPM image specification to output image file*/
	fwrite(initial_image,1,ROW*COL,fp);/*write the output image data into file*/
	
	if(fp)                      /*Close the output file handle*/
	{
		fclose(fp);
		fp = NULL;
	}

	/*To convolve the input image with the sobel filters*/
	Sobel_Convolution(sobel_image,ROW,COL);
	fp = fopen("Sobel_EdegeDetection.ppm","wb+");/*open output image file*/
	if(!fp)                     /*error handling*/
	{
		printf("fopen failed for %s\n","intial_contours.ppm");/*failure to open the output file*/
		return -1;          /*return error code*/
	}
	
	fprintf(fp,"P5 %d %d 255 ",COL,ROW);/*Write the header as per PPM image specification to output image file*/
	fwrite(sobel_image,1,ROW*COL,fp);/*write the output image data into file*/
	
	if(fp)                      /*Close the output file handle*/
	{
		fclose(fp);
		fp = NULL;
	}

	ActiveContourAlgo(image,sobel_image,contour_head,ROW,COL);


	fp = fopen("final_contour.ppm","wb+");/*open output image file*/
	if(!fp)                     /*error handling*/
	{
		printf("fopen failed for %s\n","intial_contours.ppm");/*failure to open the output file*/
		return -1;          /*return error code*/
	}
	
	fprintf(fp,"P5 %d %d 255 ",COL,ROW);/*Write the header as per PPM image specification to output image file*/
	fwrite(image,1,ROW*COL,fp);/*write the output image data into file*/
	
	if(fp)                      /*Close the output file handle*/
	{
		fclose(fp);
		fp = NULL;
	}

	/*Deallocate the memory*/
	destroy_contourlist(contour_head);

	if(image)
	{
		free(image);
		image = NULL;
	}

	if(initial_image)
	{
		free(initial_image);
		initial_image = NULL;
	}

	if(sobel_image)
	{
		free(sobel_image);
		image = NULL;
	}

	return 0;
}

