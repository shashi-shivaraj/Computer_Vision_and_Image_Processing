/**********************************************************************
*
*  FILE NAME	: letter_detection.c
*
*  DESCRIPTION  : Implement thinning, branchpoint and endpoint detection
*                 to recognize letters in an image of text.
* 
*  PLATFORM		: Linux
*
*  DATE                 NAME                    REASON
*  27nd Sep,2017        ShashiShivaraju         ECE_6310_Lab_03
*                       [C88650674]
***********************************************************************/
 
/*Header file inclusions*/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>	/*included for strcmp() and memset() api*/

/*Macro Declarations*/
#undef DEBUG_MODE	/*Enable this debug macro to dump pixel values on console*/
#undef ESSEN_DEBUG_MODE

#ifdef DEBUG_MODE
#define	DEBUG_HEIGHT 24;	/*number of rows*/
#define DEBUG_WIDTH  26; 	/*number of columns*/
#define DEBUG_HEIGHT_TEMPLATE 15 /*number of rows of template image*/
#define DEBUG_WIDTH_TEMPLATE 9  /*number of cols of template image*/
#endif /*DEBUG_MODE*/

/*Function Prototypes*/
void extract_8_neighbour_pixels(
								unsigned char *binary_image,
								int row,
								int col,
								unsigned char *threeXthree_pixels,
								int index);
								
int detect_8_neighbour_pixels(
								unsigned char *threeXthree_pixels,
								int *edge2non_egde_transistion,
								int *neighbour_edge_count,
								int *north_nonedge,
								int *east_nonedge ,
								int *south_nonedge,
								int *west_nonedge);

void thin_2_single_pixel(
                          unsigned char *binary_image,
                          int row,
                          int col);
							
void detect_endpoints_branchpoints(
									unsigned char *binary_image,
									int row,
									int col,
									int *a_endpoint_count,
									int *a_branchpoint_count);



int main(int argc,char* argv[])/*main function of the program*/
{
	
	FILE* fp = NULL,*fp_2 = NULL;                /*File pointer for file operations*/	
	
	unsigned char *image = NULL;	/*pointer to store input image data*/
	unsigned char *template = NULL; /*pointer to store input template data*/
	unsigned char *MSF_image = NULL; 	/*pointer to store Normalized MSF Normalized output image data */
	unsigned char *binary_image = NULL;/*pointer to store the data of the binary output image*/
	unsigned char *detected_image = NULL;/*pointer to store the data of the detected character from input image*/
	unsigned char *detected_binary_image = NULL;/*pointer to store the binary data of the detected character*/
	
	char magic[10];                 /*array to store header of the input image*/
	char magic_template[10];        /*array to store header of the input template image*/
	char magic_MSF[10];
	int row=0,row_template=0,row_MSF=0;       /*variables to store height of input image & template image*/
	int col=0,col_template=0,col_MSF=0;       /*variable to store width of input image & template image*/
	int max_pixel=0,max_pixel_template=0,max_pixel_MSF=0; /*maximum pixel value of the image & & template image*/
	
	int threshold = 0 ;				/*variable to store pixel threshold*/
	int ret=0;                      /*variable to store return values of API*/
	int sum=0,count=0;			/*variables to store sum and detection count values respectively*/
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
	
	int gt_col_init = 0,gt_row_init = 0,init_index = 0;
	
	int detection_flag = 0;         /*1 = detected 0=undetected*/
	int branchpoint_count = 0,endpoint_count = 0; /*variables to store branchpoint count and endpoint count*/
							
	if(6 != argc)                   /*error handling*/
	{
		printf("usage is ./[exe] [image_file] [template_file] [ground_truth_file] [pixel threshold][MSF Image]\n");/*display proper usage for execution*/
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
		printf("\nHeader info of the opened input PPM file is max pixel = %d,\
col = %d,row = %d, magic code = %s\n\n",max_pixel,col,row,magic);/*Display header info of image*/
#endif /*DEBUG_MODE*/
	


	image = (unsigned char *)calloc(row*col,sizeof(unsigned char));/*Allocate memory to store input image data*/
	if(!image)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
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
	
	ret = fscanf(fp,"%s %d %d %d"
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
	
	 printf("\nHeader info of the opened input template PPM file is max pixel = %d,\
col = %d,row = %d, magic code = %s\n\n",max_pixel_template,col_template,row_template,magic_template);/*Display header info of image*/
			
#endif /*DEBUG_MODE*/
	
   
	template = (unsigned char *)calloc(row_template*col_template,sizeof(unsigned char));/*Allocate memory to store input template image data*/
	if(!template)			/*Error handling*/
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

    fp = fopen(argv[5],"r");        /*open input image file provided as cmd line arg*/
	if(!fp)                         /*error handling*/
	{
		printf("fopen failed for %s\n",argv[5]);/*failure to open the input file*/
		return -1;              /*return error code*/	
	}
	
	ret = fscanf(fp,"%s %d %d %d "
	,magic_MSF,&col_MSF,&row_MSF,&max_pixel_MSF);	/*read header information of the image*/
	if(	4 != ret || 
		255 != max_pixel_MSF ||
		0 != strcmp("P5",magic_MSF))/*error handling specific to 8bit greyscale PPM image*/
	{
		printf("Not a greyscale image of PPM format\n");/*Not 8bit greyscale PPM image */
		return -1;              /*return error code*/	
	}

	MSF_image = (unsigned char *)calloc(row*col,sizeof(unsigned char));/*Allocate memory to store output image data*/
	if(!MSF_image)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;		/*return error code*/
	}
	
	ret = fread(MSF_image,1,row_MSF*col_MSF,fp);/*read the image data form file and store in a buffer*/
	if(ret != row_MSF*col_MSF)              /*check for invalid fread*/
	{
		printf("fread failed to read %d data from file",row_template*col_template); /*fread operation failed*/
		return -1;              /*return error code*/
	}

	if(fp)                          /*close the file handle of  input file*/
	{
		fclose(fp);
		fp = NULL;
	}

	/*Threshold the original image Image at 128 */
    for(i=0;i<row*col;i++)
    {
		/*letter is in black colour range and background is in white colour range*/
		if(128 < image[i])
		{
			binary_image[i] = 0;
		}
		else
		{ 
			binary_image[i] = 255;
		}
    }

    fp = fopen("Threshold_output.ppm","w+");/*open output image file*/
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

/*thin the thresholded Image to single pixel wide component image */	
thin_2_single_pixel(binary_image,
                          row,
                          col);	
	
/*Read the ground truth file*/    
    fp = fopen(argv[3],"r");        /*open input ground truth file provided as cmd line arg*/
	if(!fp)                         /*error handling*/
	{
		printf("fopen failed for %s\n",argv[1]);/*failure to open the input file*/
		return -1;              /*return error code*/	
	}
	
	/*Allocate memory to store the detected character data*/
	detected_image = (unsigned char *)calloc(row_template*col_template,sizeof(unsigned char));
	if(!detected_image)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
	}

	/*Allocate memory to store the detected character data*/
	detected_binary_image = (unsigned char *)calloc(row_template*col_template,sizeof(unsigned char));
	if(!detected_binary_image)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
	}

	while(0<fscanf(fp,"%c %d %d ",&gt_char,&gt_col,&gt_row))/*will exit at EOF or fscanf error*/
	{
		detection_flag = 0;
		/*Check a 9 x 15 pixel area centered at the ground truth location of the MSF_image file to detect the character*/
		for(R=gt_row-(row_template/2);R<=gt_row+(row_template/2);R++)
		{
			for(C=gt_col-(col_template/2);C<=gt_col+(col_template/2);C++)
			{
		       if(threshold < MSF_image[R*col+C])
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

#ifdef ESSEN_DEBUG_MODE
			printf("\nGT char=%c,col = %d,row = %d\n",gt_char,gt_col,gt_row);
#endif /*ESSEN_DEBUG_MODE*/

			/*
			 * Create a 9 x 15 pixel image centered at
			 * the ground truth location from the original
			 *  image of detected letter 
			 */
			 gt_row = gt_row +1;

			 gt_col_init = gt_col - col_template/2;
			 gt_row_init = gt_row - row_template/2;
			 init_index = gt_row_init*col + gt_col_init;
			 
			 for(i=0;i<row_template*col_template;i++)
			 {
				 if(0 == (i%col_template) && i!=0)
					init_index = init_index+col-col_template;
				 detected_image[i] = binary_image[init_index];
				 init_index++;
				 
			 }
		
			 //i=0;
			 //for(R=gt_row-(row_template/2);R<=gt_row+(row_template/2);R++)
			 //{
				 //for(C=gt_col-(col_template/2);C<=gt_col+(col_template/2);C++)
				 //{
					//detected_image[i] = image[R*col+C];
					//i++;
			     //}
			 //}
			
			 
			 //fp_2 = fopen("Detected_Character.ppm","w+");/*open output image file*/
			 //if(!fp_2)                     /*error handling*/
			 //{
				 //printf("fopen failed for %s\n","Binary_output.ppm");/*failure to open the output file*/
				 //return -1;                  /*return error code*/
			 //}
			 
			 //fprintf(fp_2,"P5 %d %d 255 ",col_template,row_template);/*Write the header as per PPM image specification to output image file*/
			 //fwrite(detected_image,1,row_template*col_template,fp_2);/*write the output image data into file*/
	
	         //if(fp_2)                      /*Close the output file handle*/
	         //{
		       //fclose(fp_2);
		       //fp_2 = NULL;
	         //}
 
 
			 /*detect the number of endpoints and branchpoints*/
			 detect_endpoints_branchpoints(detected_image,
											row_template,
											col_template,
									        &endpoint_count,
									        &branchpoint_count);
									        
				
									        
			/*TPR and FPR calculated based on branchpoint and endpoint detection for e*/					        
			if(1 == endpoint_count && 
			   1 == branchpoint_count)
			{
				if('e' == gt_char)
				{
					TP_Count++; /*detected and the letter is 'e'*/
				}
				else
				{
					FP_Count++; /*detected and the letter is not'e'*/
				}
			}
			else
			{
				if('e' == gt_char)
				{
					FN_Count++; /*not detected and the letter is 'e'*/
				}
				else
				{
					TN_Count++; /*not detected and the letter is not 'e'*/
				}
			}
			 
		}
		else
		{
			if('e' == gt_char)
			{
				FN_Count++; /*not detected and the letter is 'e'*/
			}
			else
			{
				TN_Count++; /*not detected and the letter is not 'e'*/
			}
		}
			
		detection_flag = 0;
	}
	
    TP_Rate  = ((float)TP_Count/(float)(TP_Count+FN_Count));
	FP_Rate  = ((float)FP_Count/(float)(FP_Count+TN_Count));

	printf("\nOutput for Threshold T = %d TP_Rate = %f FP_Rate = %f TP_Count = %d FN_Count =%d  FP_Count = %d TN_Count = %d\n",
	threshold,TP_Rate,FP_Rate,TP_Count,FN_Count,FP_Count,TN_Count);
	
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}

/********************************Resource Deallocations********************************************/
	if(image)
	{
		free(image);
		image = NULL;
	}
	
	if(MSF_image)
	{
		free(MSF_image);
		MSF_image = NULL;
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
	
	if(MSF_image)
    {
		free(MSF_image);
		MSF_image = NULL;
	}
	
	if(detected_image)
	{
		free(detected_image);
		detected_image = NULL;
	}
	
	if(detected_binary_image)
	{
		free(detected_binary_image);
		detected_binary_image = NULL;
	}
	
	return 0;                           /*return success code zero*/
}


void extract_8_neighbour_pixels(
								unsigned char *binary_image,
								int row,int col,
								unsigned char *threeXthree_pixels,
								int index)
{
	int i = index;
	
	/*check 8 neighbour pixels to determine erasal*/

	if(0 < i-1 && (i%col))
		threeXthree_pixels[0] = binary_image[i-1]; /*west[1]*/
	else
		threeXthree_pixels[0] = 0; /*represents center is border pixel*/
			 
	if(0 < i-col-1 && (i%col))
		threeXthree_pixels[1] = binary_image[i-col-1]; /*north_west[2]*/
	else
		threeXthree_pixels[1] = 0; /*represents center is border pixel*/
			 
	if(0 < i-col)
		threeXthree_pixels[2] = binary_image[i-col]; /*north[2]*/
	else
		threeXthree_pixels[2] = 0; /*represents center is border pixel*/
			 
	if(0 < i-col+1 && ((i+1)%col))
		threeXthree_pixels[3] = binary_image[i-col+1]; /*north_east[4]*/
	else
		threeXthree_pixels[3] = 0; /*represents center is border pixel*/
				
	if(i+1 < row*col && ((i+1)%col))
		threeXthree_pixels[4] = binary_image[i+1]; /*east[5]*/
	else
		threeXthree_pixels[4] = 0; /*represents center is border pixel*/
				
	if(i+col+1 < row*col && ((i+1)%col))
		threeXthree_pixels[5] = binary_image[i+col+1]; /*south_east[6]*/
	else
		threeXthree_pixels[5] = 0; /*represents center is border pixel*/
				
	if(i+col < row*col)
		threeXthree_pixels[6] = binary_image[i+col]; /*south[7]*/
	else
		threeXthree_pixels[6] = 0; /*represents center is border pixel*/
			
	if(i+col-1 < row*col && (i%col))
		threeXthree_pixels[7] = binary_image[i+col-1]; /*south_west[8]*/
	else
		threeXthree_pixels[7] = 0; /*represents center is border pixel*/
				
}

int detect_8_neighbour_pixels(
								unsigned char *threeXthree_pixels,
								int *a_edge2non_egde_transistion,
								int *a_neighbour_edge_count,
								int *a_north_nonedge,
								int *a_east_nonedge ,
								int *a_south_nonedge,
								int *a_west_nonedge)
{
	int cur = 0,prev = 0,next = 0,j = 0,k=0,next_k = 0;
	int edge2non_egde_transistion = 0,neighbour_edge_count = 0;
	int north_nonedge = 0,east_nonedge = 0,south_nonedge = 0,west_nonedge = 0;
	
	if(!threeXthree_pixels)
	{
		printf("input is NULL %p",threeXthree_pixels);
		return -1; /*return error code*/
	}
	
	prev = threeXthree_pixels[0];	
	for(j=0;j<8;j++)
	{
		cur = threeXthree_pixels[j];
		if(255 == cur)
		{
			neighbour_edge_count++;
									 
			///*check for edge to non edge transition from south_west to west */
			if(7 == j && 0 == threeXthree_pixels[0])
				edge2non_egde_transistion ++;
					 
			prev = cur;
		}
		else if(0 == cur)
		{
			/*check for edge to non edge transition*/
			if(255 == prev)
				edge2non_egde_transistion++;
					
			if(2 == j)
				north_nonedge = 1;
			else if(4 == j)
				east_nonedge = 1;
			else if(6 == j)
				south_nonedge = 1;
			else if(0 ==j)
				west_nonedge = 1;

			prev = cur;
		}
		else if(1 == cur)/*neighbour pixel is not inside the image*/
		{
			/*check for edge to non edge transition at border clockwise (from south_west to west) */
			if(7 == j && 255 == prev)
				edge2non_egde_transistion ++;
										
			if(2 == j)
				north_nonedge = 1;
			else if(4 == j)
				east_nonedge = 1;
			else if(6 == j)
				south_nonedge = 1;
			else if(0 ==j)
				west_nonedge = 1;
		}
	}
	
	if(a_edge2non_egde_transistion)
		*a_edge2non_egde_transistion = edge2non_egde_transistion;
		
	if(a_neighbour_edge_count)
		*a_neighbour_edge_count = neighbour_edge_count;
	
	if(a_north_nonedge)
		*a_north_nonedge = north_nonedge;
		
	if(a_east_nonedge)
		*a_east_nonedge  = east_nonedge;
	
	if(a_south_nonedge)
		*a_south_nonedge = south_nonedge;
	
	if(a_west_nonedge)
		*a_west_nonedge = west_nonedge;
}



void thin_2_single_pixel(unsigned char *binary_image,int row,int col)
{
	/*west[1],north_west[2],north[3],north_east[4],
	 * east[5],south_east[6],south[7],south_west[8]*/
	unsigned char threeXthree_pixels[8] = {0,0,0,0,0,0,0,0};
	int i = 0,j = 0,index = 0;
	int erasal_count = 0;
	int edge2non_egde_transistion = 0,neighbour_edge_count = 0;
	int erasal_index[row*col];
	int north_nonedge = 0,east_nonedge = 0,south_nonedge = 0,west_nonedge = 0;
	
	
#ifdef ESSEN_DEBUG_MODE

	FILE *fp = NULL;
	printf("\nDetected image is :\n");
	for(i = 0;i<row*col;i++)
	{
		if(!(i%col) && i!=0)
		printf("\n");
		printf("%d\t",binary_image[i]);
	}
	printf("\n");
#endif /*ESSEN_DEBUG_MODE*/
	
	while(1)
	{
		erasal_count = 0;
		
		for(i = 0;i< row*col;i++)
		{
			if(0 == binary_image[i])
				continue;
				
			memset(threeXthree_pixels,0,8);
			/*function call to extract the neighbourhood pixels*/
			extract_8_neighbour_pixels(binary_image,row,col,threeXthree_pixels,i);
			
			/*function call to detect count of edge to non edge transition,
			 * edge pixel and nonedge pixels(NSEW directions only )*/
			detect_8_neighbour_pixels(threeXthree_pixels,
			&edge2non_egde_transistion,&neighbour_edge_count,
			&north_nonedge,&east_nonedge ,&south_nonedge,&west_nonedge);
			
			if(
			edge2non_egde_transistion == 1 &&
			3 <= neighbour_edge_count && neighbour_edge_count <= 7 &&
			(north_nonedge || east_nonedge || (south_nonedge && west_nonedge))
			)
			{
				erasal_index[erasal_count] = i;
				erasal_count ++;
			}
			
			/*reset the variables for next pixel*/
			edge2non_egde_transistion = 0;
			neighbour_edge_count = 0;
			north_nonedge = 0;
			east_nonedge = 0;
			south_nonedge = 0;
			west_nonedge = 0; 
		}
			
		for(i=0;i<erasal_count;i++)
		{
				index = erasal_index[i];
				binary_image[index] = 0;
				erasal_index[i] = 0;
		}
		
		if(!erasal_count)
			break;
	}
			
#ifdef ESSEN_DEBUG_MODE
	printf("\nSkeleton image is :\n");
	for(i = 0;i<row*col;i++)
	{
		if(!(i%col) && i!=0)
		printf("\n");
		printf("%d\t",binary_image[i]);
	}
	printf("\n");
	
	
	fp = fopen("Skeleton_output.ppm","w+");/*open output image file*/
	if(!fp)                     /*error handling*/
	{
		printf("fopen failed for %s\n","Binary_output.ppm");/*failure to open the output file*/
		return;                  /*return error code*/
	}
	
	fprintf(fp,"P5 %d %d 255 ",col,row);/*Write the header as per PPM image specification to output image file*/
	fwrite(binary_image,1,row*col,fp);/*write the output image data into file*/
	
	if(fp)                      /*Close the output file handle*/
	{
		fclose(fp);
		fp = NULL;
	}
	
#endif /*ESSEN_DEBUG_MODE*/
}


void detect_endpoints_branchpoints(
									unsigned char *binary_image,
									int row,
									int col,
									int *a_branchpoint_count,
									int *a_endpoint_count
									)
{
	/*west[1],north_west[2],north[3],north_east[4],
	 * east[5],south_east[6],south[7],south_west[8]*/
	unsigned char threeXthree_pixels[8] = {0,0,0,0,0,0,0,0};
	int edge2non_egde_transistion = 0;
	int endpoint_count    = 0;
	int branchpoint_count = 0;
	int i = 0;
	
	
	for(i = 0;i< row*col;i++)
	{
		if(binary_image[i])/*Check for only edge pixels*/
		{
			memset(threeXthree_pixels,0,8);
			/*function call to extract the neighbourhood pixels*/
			extract_8_neighbour_pixels(binary_image,row,col,threeXthree_pixels,i);
		
			/*function call to detect count of edge to non edge transition,
			 * edge pixel and nonedge pixels(NSEW directions only )*/
			detect_8_neighbour_pixels(threeXthree_pixels,
			                      &edge2non_egde_transistion,
			                      NULL,NULL,NULL,NULL,NULL);
			                      
			if(1 == edge2non_egde_transistion)
			{
				endpoint_count++;
			}
			else if(2 < edge2non_egde_transistion)
			{
				branchpoint_count++;
			}
		}
	}
	
	if(a_branchpoint_count)
		*a_branchpoint_count = branchpoint_count;
	
	if(a_endpoint_count)
		*a_endpoint_count = endpoint_count;

	//printf("\nendpoint_count is %d branchpoint_count is %d\n",
      //      endpoint_count,branchpoint_count);	
	
}
