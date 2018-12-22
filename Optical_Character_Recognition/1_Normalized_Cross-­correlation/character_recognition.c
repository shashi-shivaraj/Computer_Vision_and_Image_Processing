/**********************************************************************
*
*  FILE NAME	: character_recognition.c
*
*  DESCRIPTION  : Implement a matched filter to recognize letters in 
*                 an image of text.
* 
*  PLATFORM		: Linux
*
*  DATE                 NAME                    REASON
*  22nd Sep,2017        ShashiShivaraju         ECE_6310_Lab_02
*                       [C88650674]
***********************************************************************/
 
/*Header file inclusions*/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>	/*included for strcmp() and memset() api*/

/*Macro Declarations*/
#undef DEBUG_MODE	/*Enable this debug macro to dump pixel values on console*/

#ifdef DEBUG_MODE
#define	DEBUG_HEIGHT 24;	/*number of rows*/
#define DEBUG_WIDTH  26; 	/*number of columns*/
#define DEBUG_HEIGHT_TEMPLATE 15 /*number of rows of template image*/
#define DEBUG_WIDTH_TEMPLATE 9  /*number of cols of template image*/
#endif /*DEBUG_MODE*/


int main(int argc,char* argv[])/*main function of the program*/
{
	
	FILE* fp = NULL;                /*File pointer for file operations*/	
	
	unsigned char *image = NULL;	/*pointer to store input image data*/
	unsigned char *template = NULL; /*pointer to store input template data*/
	unsigned char *MSF_Normalized_output = NULL; 	/*pointer to store Normalized MSF Normalized output image data */
	unsigned char *binary_image = NULL;/*pointer to store the data of the binary output image*/
	int *template_zero_mean = NULL; /*pointer to store zero mean template data */
	int *MSF_image          = NULL; /*pointer to store Matched Spatial Filter (MSF) image*/
	char magic[10];                 /*array to store header of the input image*/
	char magic_template[10];        /*array to store header of the input template image*/
	
	int threshold = 0 ;				/*variable to store pixel threshold*/
	int row=0,row_template=0;       /*variables to store height of input image & template image*/
	int col=0,col_template=0;       /*variable to store width of input image & template image*/
	int max_pixel=0,max_pixel_template=0;          /*maximum pixel value of the image & & template image*/
	int ret=0;                      /*variable to store return values of API*/
	int sum=0,average=0,count=0;			/*variables to store sum and average values respectively*/
	float min=0,max=0,range=0,MSF_Pixel=0;				/*variable to store the best match in MSF image*/
	
	unsigned int FP_Count = 0;      /* detected but the letter is not ‘template’*/ 
    unsigned int TP_Count = 0;      /* detected and the letter is ‘template’)*/
	unsigned int FN_Count = 0;      /* not detected but the letter is ‘template’*/
	unsigned int TN_Count = 0;      /* not detected ans the letter is not ‘template’*/
	float        TP_Rate  = 0;		/* variable to store True Positive Rate = (TP_Count/(TP_Count+FN_Count)) */
	float        FP_Rate  = 0;		/* variable to store False Positive Rate = (FP_Count/(FP_Count+TN_Count)) */
	
	unsigned char gt_char = '0';		/*variable to store the ground truth character*/
	int gt_row = 0,gt_col = 0;      /*variable to store the location of ground truth character*/
		
	int i=0;                        /*variable used in loops*/
	int R=0,C=0,r1=0,c1=0;	        /*variables used in convolution computation loop*/
	float out = 0;                  /*variable used integer rounding operation*/ 
	
	int detection_flag = 0;         /*1 = detected 0=undetected*/
							
	if(6 != argc)                   /*error handling*/
	{
		printf("usage is ./[exe] [image_file] [template_file] [ground_truth_file] [pixel threshold][template char]\n");/*display proper usage for execution*/
		return -1;              /*return error code*/
	}
	
	threshold = atoi(argv[4]);
	if(threshold < 0 || threshold > 255)
	{
		printf("pixel threshold should be inbetween the range(0,255)");
		return -1;
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
	
	printf("\nHeader info of the opened input PPM file is max pixel = %d,\
col = %d,row = %d, magic code = %s\n\n",max_pixel,col,row,magic);/*Display header info of image*/

	image = (unsigned char *)calloc(row*col,sizeof(unsigned char));/*Allocate memory to store input image data*/
	if(!image)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
	}
	
	MSF_Normalized_output = (unsigned char *)calloc(row*col,sizeof(unsigned char));/*Allocate memory to store output image data*/
	if(!MSF_Normalized_output)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;		/*return error code*/
	}
	
	binary_image = (unsigned char *)calloc(row*col,sizeof(unsigned char));/*Allocate memory to store output image data*/
	if(!binary_image)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;		/*return error code*/
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

    fp = fopen(argv[2],"r");        /*open input image template file provided as cmd line arg*/
	if(!fp)                         /*error handling*/
	{
		printf("fopen failed for %s\n",argv[2]);/*failure to open the input file*/
		return -1;              /*return error code*/	
	}
	
	ret = fscanf(fp,"%s %d %d %d "
	,magic_template,&col_template,&row_template,&max_pixel_template);	/*read header information of the image*/
	if(	4 != ret || 
		255 != max_pixel_template ||
		0 != strcmp("P5",magic_template))/*error handling specific to 8bit greyscale PPM image*/
	{
		printf("Not a greyscale image of PPM format\n");/*Not 8bit greyscale PPM image */
		return -1;              /*return error code*/	
	}
	
#ifdef DEBUG_MODE	
	row_template = DEBUG_HEIGHT_TEMPLATE;             /*hardcoding the image size for debug analysis*/
	col_template = DEBUG_WIDTH_TEMPLATE;		
#endif /*DEBUG_MODE*/
	
    printf("\nHeader info of the opened input template PPM file is max pixel = %d,\
col = %d,row = %d, magic code = %s\n\n",max_pixel_template,col_template,row_template,magic_template);/*Display header info of image*/
	
	template = (unsigned char *)calloc(row_template*col_template,sizeof(unsigned char));/*Allocate memory to store input template image data*/
	if(!template)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
	}

    template_zero_mean = (int *)calloc(row_template*col_template,sizeof(int));/*Allocate memory to store zero mean data of the template*/
	if(!template_zero_mean)		/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
	}

	ret = fread(template,1,row_template*col_template,fp);/*read the image data form file and store in a buffer*/
	if(ret != row_template*col_template)              /*check for invalid fread*/
	{
		printf("fread failed to read %d data from file",row_template*col_template); /*fread operation failed*/
		return -1;              /*return error code*/
	}

	if(fp)                          /*close the file handle of  input file*/
	{
		fclose(fp);
		fp = NULL;
	}

#ifdef DEBUG_MODE
	printf("Template image is :\n");
	for(i = 0;i<row_template*col_template;i++)
	{
		if(!(i%col_template) && i!=0)
		printf("\n");
		printf("%d\t",template[i]);
	}
	printf("\n");
#endif /*DEBUG_MODE*/

/*Calculate the average of the pixels of template image*/
	for(i = 0;i<row_template*col_template;i++)
	{
		sum = sum + template[i];
	}
	average = sum/(row_template*col_template);
	
#ifdef DEBUG_MODE
	printf("\nTemplate Image sum is %d; average is %d\n\n",sum,average);
#endif /*DEBUG_MODE*/

/*Calculate the zero mean template*/
    for(i = 0;i<row_template*col_template;i++)
	{
		template_zero_mean[i] = template[i]-average;
	}

#ifdef DEBUG_MODE
	printf("Zero Mean Template is :\n");
	for(i = 0;i<row_template*col_template;i++)
	{
		if(!(i%col_template) && i!=0)
		printf("\n");
		printf("%d\t",template_zero_mean[i]);
	}
	printf("\n");
#endif /*DEBUG_MODE*/

    MSF_image = (int *)calloc(row*col,sizeof(int));/*Allocate memory to store zero mean data of the template*/
	if(!MSF_image)		/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
	}

/*Convolve the input image with the zero mean template*/

	 for(R=0;R<=row-(row_template);R++)
	 {
		 for(C=0;C<=col-(col_template);C++)
		 {
			 sum = 0;
			 for(r1=0;r1<row_template;r1++)
			 {
				 for(c1=0;c1<col_template;c1++)
				 {
					sum = sum +image[col*(r1+R)+(c1+C)]*template_zero_mean[r1*col_template+c1];
				 }
			 }
			 MSF_image[col*(R+row_template/2)+(C+col_template/2)]=sum;
		 }
	 }
	 
#ifdef DEBUG_MODE
	printf("MSF_image is :\n");
	for(i = 0;i<row*col;i++)
	{
		if(!(i%col) && i!=0)
		printf("\n");
		printf("%d\t",MSF_image[i]);
	}
	printf("\n");
#endif /*DEBUG_MODE*/

/*Find the max and min of the MSF image*/
max = (float)MSF_image[0];
min = (float)MSF_image[0];
for(i=0;i<row*col;i++)
{
	if(max<MSF_image[i])
	{
		max = MSF_image[i];
	}
	
	if(min>MSF_image[i])
	{
		min = MSF_image[i];
	}
}
range = max-min;

#ifdef DEBUG_MODE
printf("MSF image max = %f,min = %f,range = %f\n",max,min,range);
#endif /*DEBUG_MODE*/


/*Normalize the MSF image to 8-bits*/
for(i=0;i<row*col;i++)
{
	MSF_Pixel = (float)MSF_image[i];
	out = ((MSF_Pixel-min)/range)*255;
	/*rounding the values to the next highest integer if value above 0.5  */
	if(out-(unsigned char)out > 0.5)
	{
		MSF_Normalized_output[i] = (unsigned char)(out+1);
	}
	else
	{
		MSF_Normalized_output[i] = (unsigned char)out;
	}
}

#ifdef DEBUG_MODE
	printf("Normalised MSF_image is :\n");
	for(i = 0;i<row*col;i++)
	{
		if(!(i%col) && i!=0)
		printf("\n");
		printf("%d\t",MSF_Normalized_output[i]);
	}
	printf("\n");
#endif /*DEBUG_MODE*/
	 
	
	fp = fopen("MSF_Normalized_output.ppm","w+");/*open output image file*/
	if(!fp)                     /*error handling*/
	{
		printf("fopen failed for %s\n","MSF_Normalized_output.ppm");/*failure to open the output file*/
		return -1;                  /*return error code*/
	}
	
	fprintf(fp,"P5 %d %d 255 ",col,row);/*Write the header as per PPM image specification to output image file*/
	fwrite(MSF_Normalized_output,1,row*col,fp);/*write the output image data into file*/
	
	if(fp)                      /*Close the output file handle*/
	{
		fclose(fp);
		fp = NULL;
	}
	
/*Threshold check to create binary image/Detected Pixels Image */
    for(i=0;i<row*col;i++)
    {
	   if(threshold < MSF_Normalized_output[i])
	   {
		 binary_image[i] = 255; /*255 for white dot or 1 for representation*/
		 count++;
	   }
	   else
	   { 
		 binary_image[i] = 0;
	   }
    }

//#ifdef DEBUG_MODE    
    printf("The detected count is %d\n",count);
//#endif /*DEBUG MODE*/

    fp = fopen("Detection_output.ppm","w+");/*open output image file*/
	if(!fp)                     /*error handling*/
	{
		printf("fopen failed for %s\n","Binary_output.ppm");/*failure to open the output file*/
		return -1;                  /*return error code*/
	}
	
	fprintf(fp,"P5 %d %d 255 ",col,row);/*Write the header as per PPM image specification to output image file*/
	fwrite(binary_image,1,row*col,fp);/*write the output image data into file*/
	
	if(fp)                      /*Close the output file handle*/
	{
		fclose(fp);
		fp = NULL;
	}
	
/*Read the ground truth file*/    
    fp = fopen(argv[3],"r");        /*open input ground truth file provided as cmd line arg*/
	if(!fp)                         /*error handling*/
	{
		printf("fopen failed for %s\n",argv[1]);/*failure to open the input file*/
		return -1;              /*return error code*/	
	}
	
	while(0<fscanf(fp,"%c %d %d ",&gt_char,&gt_col,&gt_row))/*will exit at EOF or fscanf error*/
	{

#ifdef DEBUG_MODE
		printf("GT char=%c,col = %d,row = %d\n",gt_char,gt_col,gt_row);
#endif /*DEBUG_MODE*/
		detection_flag = 0;
		/*Check a 9 x 15 pixel area centered at the ground truth location of the Binary file to detect the character*/
		for(R=gt_row-(row_template/2);R<=gt_row+(row_template/2);R++)
		{
			for(C=gt_col-(col_template/2);C<=gt_col+(col_template/2);C++)
			{
		       if(binary_image[R*col+C])
		       {
				   detection_flag = 1;
				   break;
			   }
			}
			
			if(detection_flag)
			break;
		}
		
		if(detection_flag)	
		{
			if('e' == gt_char)
			{
				TP_Count++;
			}
			else
			{
				FP_Count++;
			}
		}
		else
		{
			if('e' == gt_char)
			{
				FN_Count++;
			}
			else
			{
				TN_Count++;
			}
		}
			
		detection_flag = 0;
	}
	
    TP_Rate  = ((float)TP_Count/(float)(TP_Count+FN_Count));
	FP_Rate  = ((float)FP_Count/(float)(FP_Count+TN_Count));

	printf("Output for Threshold T = %d TP_Rate = %f FP_Rate = %f TP_Count = %d FN_Count =%d  FP_Count = %d TN_Count = %d\n",
	threshold,TP_Rate,FP_Rate,TP_Count,FN_Count,FP_Count,TN_Count);
	
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}

/********************************Resource Deallocations********************************************/
	if(image)                           /*free the allocated memory for input image*/  
	{
		free(image);
		image = NULL;
	}
	
	if(MSF_Normalized_output)                          /*free the allocated memory for output image*/
	{
		free(MSF_Normalized_output);
		MSF_Normalized_output = NULL;
	}
	
	if(binary_image)
	{
		free(binary_image);
		binary_image = NULL;
	}
	
	if(template)
	{
		free(template);
		template = NULL;
	}

	if(template_zero_mean)              /*free the allocated memory for zero mean template*/
    {
		free(template_zero_mean);
		template_zero_mean = NULL;
	}
	
	if(MSF_image)              /*free the allocated memory for zero mean template*/
    {
		free(MSF_image);
		MSF_image = NULL;
	}
	
	return 0;                           /*return success code zero*/
}
