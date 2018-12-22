/**********************************************************************
*
*  FILE NAME	: functions.c
*
*  DESCRIPTION  : Contains functions used for active contour algorithm.
* 
*  PLATFORM		: Windows
*
*  DATE	               	NAME	        	  	REASON
*  29th Oct,2017        Shashi Shivaraju        ECE_6310_Lab_05
*                       [C88650674]
***********************************************************************/
#include "header.h"

#undef DEBUG_MODE

void convert_index2height_width(int index,
								int ROW,int COL,/* size of image */
								int *x_pos,int *y_pos)/* pixel position */
{
	if(index>ROW*COL || !ROW || !COL)
	{
		printf("invalid -index out of range or invalid image size\
			   index %d Image Size: ROW %d COL %d",index,ROW,COL);
		return;
	}

	*y_pos = index/COL;
	*x_pos = index -(*y_pos*COL);

}

void convert_height_width2index(int *index,
								int ROW,int COL,/* size of image */
								int x_pos,int y_pos)/* pixel position */
{
	if(!ROW || !COL)
	{
		printf("invalid image size\
			   Image Size: ROW %d COL %d",ROW,COL);
		return;
	}

	*index = y_pos * COL + x_pos;
}

contour_position* insert_contour_position(contour_position* head,int index,
										  int x_pos,int y_pos,contour_position* prev_contour)
{
	contour_position* current = NULL;
	current = (contour_position*)malloc(sizeof(contour_position));
	if(!current)
	{
		printf("memory allocation failed");
		return NULL;
	}

	current->index = index;
	current->x_pos = x_pos;
	current->y_pos = y_pos;
	current->next = NULL;

	if(prev_contour)
		prev_contour->next = current;

	return current;
}

void destroy_contourlist(contour_position* contour_head)
{

	contour_position *curr = NULL;
	FILE*fp = NULL;
	/*Deallocate the memory*/
	curr = contour_head;
	fp = fopen("final_contour.txt","w+");
	
	printf("Final Contours are:");
	while(contour_head)
	{	
		curr = contour_head;
		contour_head = curr->next;
		
		printf("Contour Col = %d Contour Row = %d\n",curr->x_pos,curr->y_pos);
		fprintf(fp,"%d %d\n",curr->x_pos,curr->y_pos);
		free (curr);
		curr = NULL;
	}

	fclose(fp);

}

void Display_Contour(unsigned char *image,int ROW,int COL,contour_position* contour_head)
{
	contour_position *curr,*next= NULL;
	int i  = 0,index =0;

	next = curr = contour_head;
	while(next)
	{	
		curr = next;
		next = curr->next;

		for(i=-3;i<=3;i++)
		{
			/*To draw row of plus*/
			index = curr->index + i;
			if(-1 <index && index < ROW*COL)
				image[index] = 0;

			/*To draw col of plus*/
			convert_height_width2index(&index,ROW,COL,curr->x_pos,curr->y_pos - i);
			if(-1 <index && index < ROW*COL)
				image[index] = 0;
		}
	}
}

int Sobel_Convolution(unsigned char *image,int ROW,int COL)
{
	int *sobel_image_x = NULL; /*pointer to store intermiediate sobel convolution data*/
	int *sobel_image_y = NULL; /*pointer to store intermiediate sobel convolution data*/
	float *sobel_image_xy = NULL; /*pointer to store intermiediate sobel convolution data*/
	/*
	-1 0 +1
	-2 0 +2
	-1 0 +1
	*/
	int Sobel_X[9] ={-1,0,1,-2,0,2,-1,0,1};

	/*
	-1 -2 -1
	0  0  0
	+1 +2 +1
	*/
	int Sobel_Y[9] ={-1,-2,-1,0,0,0,1,2,1};
	int sum_x=0,sum_y=0,R=0,C=0,r1=0,c1=0;

	int i = 0,count = 0;
	float min_value = 0,max_value = 0,range = 0,SobelPixel = 0,Sobel_Normalized = 0;


	sobel_image_x = (int *)calloc(ROW*COL,sizeof(int));/*Allocate memory to store input image data*/
	if(!sobel_image_x)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
	}

	sobel_image_y = (int *)calloc(ROW*COL,sizeof(int));/*Allocate memory to store input image data*/
	if(!sobel_image_y)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
	}

	sobel_image_xy = (float *)calloc(ROW*COL,sizeof(float));/*Allocate memory to store input image data*/
	if(!sobel_image_xy)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
	}

#ifdef DEBUG_MODE_1
	ROW = 5;
	COL = 5;
	printf("Original image is :\n");
	for(i = 0;i<ROW*COL;i++)
	{
		if(!(i%COL) && i!=0)
			printf("\n");
		printf("%d\t",image[i]);
	}
	printf("\n");

	printf("Sobel Filter X is :\n");
	for(i=0;i<3*3;i++)
	{
		if(!(i%3) && i!=0)
			printf("\n");
		printf("%d\t",Sobel_X[i]);
	}
	printf("\n");

	printf("Sobel Filter Y is :\n");
	for(i=0;i<3*3;i++)
	{
		if(!(i%3) && i!=0)
			printf("\n");
		printf("%d\t",Sobel_Y[i]);
	}
	printf("\n");
#endif /*DEBUG_MODE*/


	for(R=1;R<ROW-1;R++)              /*2D convolution of 3x3 sobel filter with the input image*/
	{
		for(C=1;C<COL-1;C++)
		{
			sum_x = 0;
			sum_y = 0;
			for(r1=-1;r1<=1;r1++)
			{
				for(c1=-1;c1<=1;c1++)
				{
					sum_x = sum_x +image[(R+r1)*COL+(C+c1)]*Sobel_X[(r1+1)*3+(c1+1)];
					sum_y = sum_y +image[(R+r1)*COL+(C+c1)]*Sobel_Y[(r1+1)*3+(c1+1)];
				}
			}
			sobel_image_x[COL*R+C]=sum_x;
			sobel_image_y[COL*R+C]=sum_y;
			sobel_image_xy[COL*R+C] = (float)sqrt(sum_x*sum_x+sum_y*sum_y);

			if(!count|| sobel_image_xy[COL*R+C] < min_value)
				min_value = sobel_image_xy[COL*R+C];

			if(!count|| sobel_image_xy[COL*R+C] > max_value)
				max_value = sobel_image_xy[COL*R+C]; 

			count ++;
		}
	}

	range = max_value-min_value;
	/*Normalize the sobel convolution result to 8-bits*/
	for(i=0;i<ROW*COL;i++)
	{
		SobelPixel = sobel_image_xy[i];
		Sobel_Normalized = ((SobelPixel-min_value)/range)*255;
		/*rounding the values to the next highest integer if value above 0.5  */
		if(Sobel_Normalized-(unsigned char)Sobel_Normalized > 0.5)
		{ 
			image[i] = (unsigned char)(Sobel_Normalized+1);
		}
		else
		{
			image[i] = (unsigned char)Sobel_Normalized;
		}
	}

#ifdef DEBUG_MODE_1

	printf("Sobel X image is :\n");
	for(i = 0;i<ROW*COL;i++)
	{
		if(!(i%COL) && i!=0)
			printf("\n");
		printf("%d\t",sobel_image_x[i]);
	}
	printf("\n");

	printf("Sobel Y image is :\n");
	for(i = 0;i<ROW*COL;i++)
	{
		if(!(i%COL) && i!=0)
			printf("\n");
		printf("%d\t",sobel_image_y[i]);
	}
	printf("\n");

	printf("Sobel XY image is :\n");
	for(i = 0;i<ROW*COL;i++)
	{
		if(!(i%COL) && i!=0)
			printf("\n");
		printf("%.2f\t",sobel_image_xy[i]);
	}
	printf("\n");

	printf("The min value is %f,the max value is %f",min_value,max_value);
#endif /*DEBUG_MODE*/

	if(sobel_image_x)
		free(sobel_image_x);
	sobel_image_x = NULL;

	if(sobel_image_y)
		free(sobel_image_y);
	sobel_image_y = NULL;

	if(sobel_image_xy)
		free(sobel_image_xy);
	sobel_image_xy = NULL;

	return 0;
}

void ActiveContourAlgo(unsigned char *image,unsigned char *sobel_image,contour_position* contour_head,int ROW,int COL)
{
	contour_position *curr_cont = NULL,*next_cont = NULL;
	float *internal_energy_1 = NULL;/*To find square of distance between contour points*/
	float *internal_energy_2 = NULL;/*To find square of (avrg contour distance - distance between contour points)*/
	float *external_energy = NULL;/*To find the image gradient magnitude*/
	float *total_energy = NULL;/*To find the total enrgy in 7x7 window*/
	float ext_eng =0,int_eng1 = 0,int_eng2 = 0,total_eng = 0;
	int i=0,j=0,count=0,r1=0,c1=0;
	int contour_head_xpos = 0,contour_head_ypos = 0;
	int cur_xpos = 0,cur_ypos = 0,next_xpos = 0,next_ypos = 0,index = 0;
	float avrg_contour_distance = 0,distance_square = 0,distance_diff_square = 0;

	float max_energy1 = 0,min_energy1 = 0,max_energy2 = 0,min_energy2 = 0;
	float max_ext_energy = 0,min_ext_energy = 0,sobel_pixel_square = 0;
	float min_total_energy = 0,energy1_range = 0,energy2_range = 0,ext_energy_range = 0;

	

	internal_energy_1 = (float*)calloc(7*7,sizeof(float));
	if(!internal_energy_1)
	{
		printf("memory allocation failed\n");
		return;
	}

	internal_energy_2 = (float*)calloc(7*7,sizeof(float));
	if(!internal_energy_2)
	{
		printf("memory allocation failed\n");
		return;
	}

	external_energy = (float*)calloc(7*7,sizeof(float));
	if(!external_energy)
	{
		printf("memory allocation failed\n");
		return;
	}

	total_energy = (float*)calloc(7*7,sizeof(float));
	if(!total_energy)
	{
		printf("memory allocation failed\n");
		return;
	}

	for(i=0;i<ROW*COL;i++)
	{
		sobel_pixel_square = SQR(sobel_image[i]);
		if(!i || sobel_pixel_square < min_ext_energy)
			min_ext_energy = sobel_pixel_square;

		if(!i|| sobel_pixel_square > max_ext_energy)
			max_ext_energy = sobel_pixel_square;

	}

	for(i=0;i<CONTOUR_ITERATIONS;i++)
	{
		contour_head_xpos = contour_head->x_pos;
		contour_head_ypos = contour_head->y_pos;

		/*To calculate the average distance between the contour points*/
			CalculateAverageContourDistance(contour_head,&avrg_contour_distance);

		/*To calculate the internal energies and
		external energy at each contour point around 7x7 pixel window*/
		curr_cont = next_cont = contour_head;
		while(next_cont)
		{	

			curr_cont = next_cont;
			next_cont = curr_cont->next;

			cur_xpos = curr_cont->x_pos;
			cur_ypos = curr_cont->y_pos;

			if(next_cont)
			{
				next_xpos = next_cont->x_pos;
				next_ypos = next_cont->y_pos;
			}
			else /*Assume the last contour point connects first contour point*/
			{
				next_xpos = contour_head_xpos;
				next_ypos = contour_head_ypos;
			}

			count = 0;
			for(r1=-3;r1<=3;r1++)
			{
				 for(c1=-3;c1<=3;c1++)
				 {
					/*Internal Energy 1
					(Square of distance between contour points)*/
					distance_square = SQR(next_xpos-(cur_xpos+c1))+\
						SQR(next_ypos-(cur_ypos+r1));
					internal_energy_1[count] = distance_square;

					if(!count|| distance_square < min_energy1)
						min_energy1 = distance_square;

					if(!count|| distance_square > max_energy1)
						max_energy1 = distance_square;

					/*Internal Energy 2
					(Square of difference between average contour distance
					and the distance between current contour points )*/
					distance_diff_square = SQR(avrg_contour_distance - sqrt(distance_square));
					internal_energy_2[count] = distance_diff_square;

					if(!count || distance_diff_square < min_energy2)
						min_energy2 = distance_diff_square;

					if(!count|| distance_diff_square > max_energy2)
						max_energy2 = distance_diff_square;

					/*External Energy
					(Square of the image gradient magnitude(Sobel convoluted image))*/
					convert_height_width2index(&index,ROW,COL,cur_xpos+c1,cur_ypos+r1);
					sobel_pixel_square = SQR(sobel_image[index]);
					external_energy[count] = sobel_pixel_square;

					count ++;
				 }
			}

			count = 0;min_total_energy = 0;
			ext_eng =0;int_eng1 = 0;int_eng2 = 0;total_eng = 0;

			/*Normalise the individual energies and find total energy*/
			energy1_range	  = max_energy1 - min_energy1;
			energy2_range	  = max_energy2 - min_energy2;  
			ext_energy_range  = max_ext_energy - min_ext_energy;

			for(j=0;j<49;j++)
			{
				int_eng1 = ((internal_energy_1[j]-min_energy1)/energy1_range)*1;
				int_eng2 = ((internal_energy_2[j]-min_energy2)/energy2_range)*1;
				ext_eng = ((external_energy[j]-min_ext_energy)/ext_energy_range)*1;
				
				total_eng = int_eng1+int_eng2-ext_eng;
				total_energy[j] = total_eng;
				if(!j || total_eng < min_total_energy)
				{
						min_total_energy = total_eng;
						count = j;/*location of lowest total energy in window*/
				}
			}
			
			/*udpate the new contour position to the lowest total energy location in 7x7 window*/
			convert_index2height_width(count,7,7,&next_xpos,&next_ypos);
			curr_cont->x_pos = curr_cont->x_pos + next_xpos -3;
			curr_cont->y_pos = curr_cont->y_pos + next_ypos -3;
			//if(next_xpos>3)
				//curr_cont->x_pos = curr_cont->x_pos + (next_xpos-3);
			//else if(next_xpos<3)
				//curr_cont->x_pos = curr_cont->x_pos - (next_xpos+3);

			//if(next_ypos>3)
				//curr_cont->y_pos = curr_cont->y_pos + (next_ypos-3);
			//else if(next_ypos<3)
				//curr_cont->y_pos = curr_cont->y_pos + (next_ypos-3);

			 convert_height_width2index(&curr_cont->index,ROW,COL,curr_cont->x_pos,curr_cont->y_pos);

#ifdef DEBUG_MODE

			printf("internal_energy 1 is :\n");
			for(i = 0;i<7*7;i++)
			{
				if(!(i%7) && i!=0)
					printf("\n");
				printf("%.2f\t",internal_energy_1[i]);
			}
			printf("\n");

			printf("internal_energy 2 is :\n");
			for(i = 0;i<7*7;i++)
			{
				if(!(i%7) && i!=0)
					printf("\n");
				printf("%.2f\t",internal_energy_2[i]);
			}
			printf("\n");

			printf("external_energy is :\n");
			for(i = 0;i<7*7;i++)
			{
				if(!(i%7) && i!=0)
					printf("\n");
				printf("%.2f\t",external_energy[i]);
			}
			printf("\n");

			printf("total_energy is :\n");
			for(i = 0;i<7*7;i++)
			{
				if(!(i%7) && i!=0)
					printf("\n");
				printf("%.2f\t",total_energy[i]);
			}
			printf("\n");

#endif 

		}
	}

	/*To display final contour position on top of the image using plus marks*/
	Display_Contour(image,ROW,COL,contour_head);
	
	/*Deallocate the memory*/
	if(internal_energy_1)
	{
		free(internal_energy_1);
		internal_energy_1 = NULL;
	}

	if(internal_energy_2)
	{
		free(internal_energy_2);
		internal_energy_2 = NULL;
	}

	if(external_energy)
	{
		free(external_energy);
		external_energy = NULL;
	}

	if(total_energy)
	{
		free(total_energy);
		total_energy = NULL;
	}
}

void CalculateAverageContourDistance(contour_position* contour_head,float *avrg_distance)
{
	contour_position *curr_cont = NULL,*next_cont = NULL;
	float distance_sum = 0;
	int next_xpos = 0,next_ypos = 0;
	int count = 0;

	/*To calculate average distance between all countour points*/
	curr_cont = next_cont = contour_head;
	while(next_cont)
	{	
		curr_cont = next_cont;
		next_cont = curr_cont->next;
		if(next_cont)
		{
			next_xpos = next_cont->x_pos;
			next_ypos = next_cont->y_pos;
		}
		else /*Assume the last contour point connects first contour point*/
		{
			next_xpos = contour_head->x_pos;
			next_ypos = contour_head->y_pos;
		}
		count ++;

		distance_sum = distance_sum +\
			sqrt(SQR(curr_cont->x_pos-next_xpos)+SQR(curr_cont->y_pos-next_ypos));
	}
	*avrg_distance = distance_sum/count;
}
