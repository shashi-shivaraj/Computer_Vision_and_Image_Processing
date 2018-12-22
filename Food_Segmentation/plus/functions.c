#include "header.h"

int convert_index2height_width(int index,
							   int ROW,int COL,/* size of image */
							   int *x_pos,int *y_pos)/* pixel position */
{
	if(index>ROW*COL || !ROW || !COL)
	{
		/*printf("invalid -index out of range or invalid image size\
		index %d Image Size: ROW %d COL %d",index,ROW,COL);*/
		return -1;
	}

	*y_pos = index/COL;
	*x_pos = index -(*y_pos*COL);
	return 0;

}

int convert_height_width2index(int *index,
							   int ROW,int COL,/* size of image */
							   int x_pos,int y_pos)/* pixel position */
{
	if(!ROW || !COL || x_pos >=COL || y_pos >= ROW )
	{
		/*printf("invalid image size\
		Image Size: ROW %d COL %d",ROW,COL);*/
		return -1;
	}

	*index = y_pos * COL + x_pos;
	return 0;
}

int convert_height_width2rgbindex(int *index,
								  int ROW,int COL,/* size of image */
								  int x_pos,int y_pos)/* pixel position */
{
	if(!ROW || !COL || x_pos >=(COL*3) || y_pos >= ROW )
	{
		/*printf("invalid image size\
		Image Size: ROW %d COL %d",ROW,COL);*/
		return -1;
	}

	*index = y_pos * COL * 3+ x_pos*3;
	return 0;
}

void convert_rgb_to_grey(unsigned char *image,unsigned char *grey_image,int ROW,int COL)
{
	FILE* fp = NULL;
	int i=0,j = 0;
	float pixel_avg = 0; 

	while(i+2 < ROW*COL*3)
	{
		pixel_avg =(float)(image[i]+image[i+1]+image[i+2])/3;
		if(255 <pixel_avg)
			pixel_avg = 255;
		grey_image[j]= (unsigned char)pixel_avg;

		i = i+3;
		j = j+1;
	}

	fp = fopen("grey_image.ppm","wb+");
	fprintf(fp,"P5 %d %d 255 ",COL,ROW);/*Write the header as per PPM image specification to output image file*/
	fwrite(grey_image,1,ROW*COL,fp);/*write the output image data into file*/

	if(fp)                      /*Close the output file handle*/
	{
		fclose(fp);
		fp = NULL;
	}
}

void downsample_rgb_image(unsigned char *image,unsigned char *output,int ROW,int COL)
{
	FILE* fp = NULL;
	int i=0,j = 0;
	int r = 0,c = 0;

	for(r=0;r<ROW;r=r+2)
	{
		for(c=0;c<COL*3;c=c+6)
		{
			output[j]=image[r*COL*3+c];
			output[j+1]=image[r*COL*3+c+1];
			output[j+2]=image[r*COL*3+c+2];

			/*output[j]=200;
			output[j+1]=100;
			output[j+2]=50;*/

			j=j+3;
		}
	}

	fp = fopen("reduced_image.pnm","wb+");
	fprintf(fp,"P6 %d %d 255 ",COL/2,ROW/2);/*Write the header as per PPM image specification to output image file*/
	fwrite(output,1,3*ROW/2*COL/2,fp);/*write the output image data into file*/

	if(fp)                      /*Close the output file handle*/
	{
		fclose(fp);
		fp = NULL;
	}
}

void downsample_grey_image(unsigned char *image,unsigned char *output,int ROW,int COL)
{
	int r = 0,c = 0;
	int j = 0;

	for(r=0;r<ROW;r=r+2)
	{
		for(c=0;c<COL;c=c+2)
		{
			output[j]=image[r*COL+c];
			j++;
		}
	}
}

int Sobel_Convolution(unsigned char *image,int ROW,int COL)
{
	FILE *fp = NULL;
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

	fp = fopen("./Sobel_Convolution.ppm","wb+");/*open output image file*/
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


void destroy_list(contour_position* list_head)
{

	contour_position *curr = NULL;

	/*Deallocate the memory*/
	curr = list_head;
	while(list_head)
	{	
		curr = list_head;
		list_head = curr->next;
		free (curr);
		curr = NULL;
	}
}

void reset_contour_movestate(contour_position* list_head)
{

	contour_position *head = NULL,
		*curr_cont = NULL,
		*next_cont = NULL;

	int i=0,j=0,index=0,ret = 0;

	head = (contour_position *)list_head;

	curr_cont = next_cont = list_head;
	while(next_cont)
	{
		curr_cont = next_cont;
		next_cont = curr_cont->next;

		if(curr_cont->pos_state == eFixed)
		{
			curr_cont->pos_state = eMovable;
		}
	}
}

int MutexLock(void* h_Mutex)
{
	DWORD lresult = 0;

	lresult = WaitForSingleObject((HANDLE)h_Mutex,INFINITE);
	switch(lresult)
	{
	case WAIT_OBJECT_0:
		{
			return 0;
		}break;
	case WAIT_TIMEOUT:
		{
			return -1;
		}break;
	case WAIT_FAILED:
		{	
			return -1;
		}
	}
	return 0;
}

int MutexUnLock(void* h_Mutex)
{

	if(!ReleaseMutex((HANDLE)h_Mutex))
	{
		return -1;
	}
	return 0;
}

void RubberBandActiveContourAlgo(unsigned char *image,unsigned char *sobel_image,
								  unsigned char *rgb_image,contour_position* contour_head,int ROW,int COL,
								  Algo_options* AlgoOpt)
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
	float min_total_energy = 0,energy1_range = 0,energy2_range = 0,ext_energy_range = 0,ext_energy_2_range = 0;

	float *external_energy_2 = NULL,max_ext_eng_2 = 0,min_ext_eng_2 = 0,ext_energy_2 = 0;/*To find the image intensity variance*/
	float intensity_sum = 0,intensity_mean = 0,variance = 0;
	float red_avg = 0,green_avg =0,blue_avg =0;
	float centroid_xpos = 0,centroid_ypos = 0;

	int ret = 0;
	int LEFT_CLICK_WINDOW = 0;
	LEFT_CLICK_WINDOW = AlgoOpt->left_click_window;



	internal_energy_1 = (float*)calloc(LEFT_CLICK_WINDOW*LEFT_CLICK_WINDOW,sizeof(float));
	if(!internal_energy_1)
	{
		printf("memory allocation failed\n");
		return;
	}

	internal_energy_2 = (float*)calloc(LEFT_CLICK_WINDOW*LEFT_CLICK_WINDOW,sizeof(float));
	if(!internal_energy_2)
	{
		printf("memory allocation failed\n");
		return;
	}

	external_energy = (float*)calloc(LEFT_CLICK_WINDOW*LEFT_CLICK_WINDOW,sizeof(float));
	if(!external_energy)
	{
		printf("memory allocation failed\n");
		return;
	}

	external_energy_2 = (float*)calloc(LEFT_CLICK_WINDOW*LEFT_CLICK_WINDOW,sizeof(float));
	if(!external_energy_2)
	{
		printf("memory allocation failed\n");
		return;
	}

	total_energy = (float*)calloc(LEFT_CLICK_WINDOW*LEFT_CLICK_WINDOW,sizeof(float));
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

	for(i=0;i<1;i++)
	{
		contour_head_xpos = contour_head->x_pos;
		contour_head_ypos = contour_head->y_pos;

		/*To calculate the average distance between the contour points*/
		CalculateAverageContourDistance(contour_head,&avrg_contour_distance);

		/*To claculate the centroid of the countour points*/
		CalculateContourCentroid(contour_head,&centroid_xpos,&centroid_ypos);
		ret = convert_height_width2rgbindex(&index,ROW,COL,centroid_xpos,centroid_ypos);
		if(ret != 0 || 0 > index )
			continue;

		red_avg = rgb_image[index];
		green_avg = rgb_image[index+1];
		blue_avg = rgb_image[index+2];

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
			for(r1=-(LEFT_CLICK_WINDOW/2);r1<=(LEFT_CLICK_WINDOW/2);r1++)
			{
				for(c1=-(LEFT_CLICK_WINDOW/2);c1<=(LEFT_CLICK_WINDOW/2);c1++)
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
					ret = convert_height_width2index(&index,ROW,COL,cur_xpos+c1,cur_ypos+r1);
					if(ret != 0 || 0 > index )
						continue;

					sobel_pixel_square = SQR(sobel_image[index]);
					external_energy[count] = sobel_pixel_square;

					/*External Energy 2
					(Variance of the rgb image intensity from the rgb image intensity of the centroid)*/
					ret = convert_height_width2rgbindex(&index,ROW,COL,cur_xpos+c1,cur_ypos+r1);
					if(ret != 0 || 0 > index )
						continue;

					variance = SQR(abs(red_avg-rgb_image[index])+\
						abs(green_avg-rgb_image[index+1])+abs(blue_avg-rgb_image[index+2]));
					external_energy_2[count] = variance;
					if(!count || variance < min_ext_eng_2)
						min_ext_eng_2 = variance;

					if(!count|| variance > max_ext_eng_2)
						max_ext_eng_2 = variance;

					count ++;
				}
			}

			count = 0;min_total_energy = 0;
			ext_eng =0;ext_energy_2 = 0;int_eng1 = 0;int_eng2 = 0;total_eng = 0;

			/*Normalise the individual energies and find total energy*/
			energy1_range	  = max_energy1 - min_energy1;
			energy2_range	  = max_energy2 - min_energy2;  
			ext_energy_range  = max_ext_energy - min_ext_energy;
			ext_energy_2_range = max_ext_eng_2 - min_ext_eng_2;

			for(j=0;j<(SQR(LEFT_CLICK_WINDOW));j++)
			{
				int_eng1 = ((internal_energy_1[j]-min_energy1)/energy1_range)* AlgoOpt->left_IE1_W;
				int_eng2 = ((internal_energy_2[j]-min_energy2)/energy2_range)* AlgoOpt->left_IE2_W;
				ext_eng = ((external_energy[j]-min_ext_energy)/ext_energy_range)* AlgoOpt->left_EE1_W;
				ext_energy_2 = ((external_energy_2[j]-min_ext_eng_2)/ext_energy_2_range)* AlgoOpt->left_EE2_W;

				total_eng = int_eng1+int_eng2-ext_eng+ext_energy_2;
				total_energy[j] = total_eng;
				if(!j || total_eng < min_total_energy)
				{
					min_total_energy = total_eng;
					count = j;/*location of lowest total energy in window*/
				}
			}

			/*udpate the new contour position to the lowest total energy location in 7x7 window*/
			ret = convert_index2height_width(count,LEFT_CLICK_WINDOW,LEFT_CLICK_WINDOW,&next_xpos,&next_ypos);
			if(ret != 0 || 0 > next_xpos || 0 > next_ypos )
				continue;

			curr_cont->last_x_pos = curr_cont->x_pos;
			curr_cont->last_y_pos = curr_cont->y_pos;

			if(next_xpos>(LEFT_CLICK_WINDOW/2))
				curr_cont->x_pos = curr_cont->x_pos + (next_xpos-(LEFT_CLICK_WINDOW/2));
			else if(next_xpos<(LEFT_CLICK_WINDOW/2))
				curr_cont->x_pos = curr_cont->x_pos - (next_xpos+(LEFT_CLICK_WINDOW/2));

			if(next_ypos>(LEFT_CLICK_WINDOW/2))
				curr_cont->y_pos = curr_cont->y_pos + (next_ypos-(LEFT_CLICK_WINDOW/2));
			else if(next_ypos<(LEFT_CLICK_WINDOW/2))
				curr_cont->y_pos = curr_cont->y_pos + (next_ypos-(LEFT_CLICK_WINDOW/2));

			ret = convert_height_width2index(&curr_cont->index,ROW,COL,curr_cont->x_pos,curr_cont->y_pos);
			if(ret != 0 || 0 > curr_cont->index )
				continue;

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

	if(external_energy_2)
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

void CalculateContourCentroid(contour_position* contour_head,float *centroid_xpos,float *centroid_ypos)
{
	contour_position *curr_cont = NULL,*next_cont = NULL;
	float xpos_sum = 0,ypos_sum = 0;
	int count = 0;

	curr_cont = next_cont = contour_head;
	while(next_cont)
	{	
		curr_cont = next_cont;
		next_cont = curr_cont->next;

		count ++;

		xpos_sum = xpos_sum +curr_cont->x_pos;
		ypos_sum = ypos_sum +curr_cont->y_pos;
	}

	*centroid_xpos = xpos_sum/count;
	*centroid_ypos = ypos_sum/count;
}


void CalculateAverageContourRGBIntensity(contour_position* contour_head,unsigned char *rgb_image,int ROWS,int COLS,
										 float *avrg_red,float *avrg_green,float *avrg_blue)
{
	contour_position *curr_cont = NULL,*next_cont = NULL;
	int red =0 ,green = 0,blue =0;
	float red_sum = 0,green_sum = 0,blue_sum = 0;
	int next_xpos = 0,next_ypos = 0;
	int count = 0,index = 0,ret = 0;

	/*To calculate average distance between all countour points*/
	curr_cont = next_cont = contour_head;
	while(next_cont)
	{	
		curr_cont = next_cont;
		next_cont = curr_cont->next;

		count ++;

		ret = convert_height_width2rgbindex(&index,ROWS,COLS,curr_cont->x_pos,curr_cont->y_pos);
		if(ret != 0 || 0 > index )
			continue;

		red = rgb_image[index];
		red_sum = red_sum + red;

		green = rgb_image[index+1];
		green_sum = green_sum + green;

		blue = rgb_image[index+2];
		blue_sum = blue_sum+blue;
	}

	*avrg_red = red_sum/count;
	*avrg_green = green_sum/count;
	*avrg_blue = blue_sum/count;
}

void update_contour_display(contour_position* contour_head,int ROWS,int COLS,unsigned char *labels,int option)
{
	contour_position *list_head = NULL,
		*curr_cont = NULL,
		*next_cont = NULL,
		*prev_cont = NULL;

	int i=0,j=0,index=0,ret = 0;

	list_head = (contour_position *)contour_head;

	curr_cont = next_cont = list_head;
	while(next_cont)
	{
		curr_cont = next_cont;
		next_cont = curr_cont->next;

		/*Update the contour on the screen*/
		for(i=-2;i<=2;i++)
		{
			for(j=-2;j<=2;j++)
			{
				///*erase the previous contour first*/
				//ret = convert_height_width2index(&index,ROWS,COLS,curr_cont->last_x_pos+i,curr_cont->last_y_pos+j);
				//if(ret != 0)
				//	continue;
				//labels[index] = 0;

				/*display the new contour now*/
				ret = convert_height_width2index(&index,ROWS,COLS,curr_cont->x_pos+i,curr_cont->y_pos+j);
				if(ret != 0 || 0 > index )
					continue;

				if(eErase == option)
				{
					labels[index] = 0;
				}
				else
				{
					labels[index] = 255;
				}
			}
		}
	}
}



void BalloonActiveContourAlgo(unsigned char *image,
							  unsigned char *sobel_image,
							  unsigned char *rgb_image,
							  contour_position* contour_head,
							  int ROW,int COL,
							  int seed_xpos,int seed_ypos,
							   Algo_options* AlgoOpt)
{

	contour_position *curr_cont = NULL,*next_cont = NULL;
	float *internal_energy_1 = NULL;/*To find curvature of the contour point*/
	float *internal_energy_2 = NULL;/*To find square of (avrg contour distance - distance between contour points)*/
	float *external_energy = NULL;/*To find the image gradient magnitude*/
	float *total_energy = NULL;/*To find the total enrgy in 7x7 window*/
	float ext_eng =0,int_eng1 = 0,int_eng2 = 0,total_eng = 0;
	int i=0,j=0,count=0,r1=0,c1=0;
	int contour_head_xpos = 0,contour_head_ypos = 0;
	int cur_xpos = 0,cur_ypos = 0,next_xpos = 0,next_ypos = 0,index = 0;
	float avrg_contour_distance = 0,distance_square = 0,distance_diff_square = 0,radius_square = 0,curve_square = 0;

	float max_energy1 = 0,min_energy1 = 0,max_energy2 = 0,min_energy2 = 0;
	float max_ext_energy = 0,min_ext_energy = 0,sobel_pixel_square = 0; 
	float min_total_energy = 0,energy1_range = 0,energy2_range = 0,ext_energy_range = 0,ext_energy_2_range = 0;

	float *external_energy_2 = NULL,max_ext_eng_2 = 0,min_ext_eng_2 = 0,ext_energy_2 = 0;/*To find the image intensity variance*/
	float intensity_sum = 0,intensity_mean = 0,variance = 0;
	float red_avg = 0,green_avg =0,blue_avg =0;
	float centroid_xpos = 0,centroid_ypos = 0;
	int ret = 0;

	float *external_energy_3 = NULL,max_ext_eng_3 = 0,min_ext_eng_3 = 0,ext_energy_3 = 0,ext_energy_3_range = 0;

	int RIGHT_CLICK_WINDOW = 0;
	RIGHT_CLICK_WINDOW = AlgoOpt->right_click_window;


	internal_energy_1 = (float*)calloc(RIGHT_CLICK_WINDOW*RIGHT_CLICK_WINDOW,sizeof(float));
	if(!internal_energy_1)
	{
		printf("memory allocation failed\n");
		return;
	}

	internal_energy_2 = (float*)calloc(RIGHT_CLICK_WINDOW*RIGHT_CLICK_WINDOW,sizeof(float));
	if(!internal_energy_2)
	{
		printf("memory allocation failed\n");
		return;
	}

	external_energy = (float*)calloc(RIGHT_CLICK_WINDOW*RIGHT_CLICK_WINDOW,sizeof(float));
	if(!external_energy)
	{
		printf("memory allocation failed\n");
		return;
	}

	external_energy_2 = (float*)calloc(RIGHT_CLICK_WINDOW*RIGHT_CLICK_WINDOW,sizeof(float));
	if(!external_energy_2)
	{
		printf("memory allocation failed\n");
		return;
	}

	external_energy_3 = (float*)calloc(RIGHT_CLICK_WINDOW*RIGHT_CLICK_WINDOW,sizeof(float));
	if(!external_energy_3)
	{
		printf("memory allocation failed\n");
		return;
	}

	total_energy = (float*)calloc(RIGHT_CLICK_WINDOW*RIGHT_CLICK_WINDOW,sizeof(float));
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

	for(i=0;i<1;i++)
	{
		contour_head_xpos = contour_head->x_pos;
		contour_head_ypos = contour_head->y_pos;

		/*To calculate the average distance between the contour points*/
		CalculateAverageContourDistance(contour_head,&avrg_contour_distance);

		/*To claculate the centroid of the countour points*/
		CalculateContourCentroid(contour_head,&centroid_xpos,&centroid_ypos);


		///*To calculate the average RGB between the contour points*/
		//CalculateAverageContourRGBIntensity(contour_head,rgb_image,ROW,COL,
		//	&red_avg,&green_avg,&blue_avg);

		ret = convert_height_width2rgbindex(&index,ROW,COL,centroid_xpos,centroid_ypos);
		if(ret != 0 || 0 > index )
			continue;

		red_avg = rgb_image[index];
		green_avg = rgb_image[index+1];
		blue_avg = rgb_image[index+2];


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
			for(r1=-(RIGHT_CLICK_WINDOW/2);r1<=(RIGHT_CLICK_WINDOW/2);r1++)
			{
				for(c1=-(RIGHT_CLICK_WINDOW/2);c1<=(RIGHT_CLICK_WINDOW/2);c1++)
				{
					/*Internal Energy 1
					(curvature of the contour point from the centroid)*/
					radius_square = SQR(centroid_xpos-(cur_xpos+c1))+\
						SQR(centroid_ypos-(cur_ypos+r1));
					curve_square = 1/radius_square;
					internal_energy_1[count] = curve_square;

					if(!count|| curve_square < min_energy1)
						min_energy1 = curve_square;

					if(!count|| curve_square > max_energy1)
						max_energy1 = curve_square;

					/*Internal Energy 2
					(Square of difference between average contour distance
					and the distance between current contour points )*/
					distance_square = SQR(next_xpos-(cur_xpos+c1))+\
						SQR(next_ypos-(cur_ypos+r1));
					distance_diff_square = SQR(avrg_contour_distance - sqrt(distance_square));
					internal_energy_2[count] = distance_diff_square;

					if(!count || distance_diff_square < min_energy2)
						min_energy2 = distance_diff_square;

					if(!count|| distance_diff_square > max_energy2)
						max_energy2 = distance_diff_square;

					/*External Energy
					(Square of the image gradient magnitude(Sobel convoluted image))*/
					ret = convert_height_width2index(&index,ROW,COL,cur_xpos+c1,cur_ypos+r1);
					if(ret != 0 || 0 > index )
						continue;
					sobel_pixel_square = SQR(sobel_image[index]);
					external_energy[count] = sobel_pixel_square;

					/*External Energy 3
					(Variance of the image intensity from 
					the average intensity around the contour points)*/
					intensity_sum = intensity_sum + image[index];

					/*External Energy 2
					(Variance of the image rgb intensity from 
					the  rgb intensity of the centroid points)*/

					ret = convert_height_width2rgbindex(&index,ROW,COL,cur_xpos+c1,cur_ypos+r1);
					if(ret != 0 || 0 > index )
						continue;

					variance = SQR(abs(red_avg-rgb_image[index])+\
						abs(green_avg-rgb_image[index+1])+abs(blue_avg-rgb_image[index+2]));
					external_energy_2[count] = variance;
					if(!count || variance < min_ext_eng_2)
						min_ext_eng_2 = variance;

					if(!count|| variance > max_ext_eng_2)
						max_ext_eng_2 = variance;

					
			

					count ++;
				}
			}

			count = 0;
			intensity_mean = intensity_sum/(RIGHT_CLICK_WINDOW*RIGHT_CLICK_WINDOW);
			for(r1=-(RIGHT_CLICK_WINDOW/2);r1<=(RIGHT_CLICK_WINDOW/2);r1++)
			{
				for(c1=-(RIGHT_CLICK_WINDOW/2);c1<=(RIGHT_CLICK_WINDOW/2);c1++)
				{
					ret = convert_height_width2index(&index,ROW,COL,cur_xpos+c1,cur_ypos+r1);
					if(ret != 0 || 0 > index )
						continue;
					variance = SQR(image[index]- intensity_mean);
					external_energy_3[count] = variance;

					if(!count || variance < min_ext_eng_3)
						min_ext_eng_3 = variance;

					if(!count|| variance > max_ext_eng_3)
						max_ext_eng_3 = variance;

					count ++;

				}
			}

			count = 0;min_total_energy = 0;variance = 0;
			ext_eng =0;ext_energy_2 = 0;ext_energy_3 = 0;int_eng1 = 0;int_eng2 = 0;total_eng = 0;

			/*Normalise the individual energies and find total energy*/
			energy1_range	  = max_energy1 - min_energy1;
			energy2_range	  = max_energy2 - min_energy2;  
			ext_energy_range  = max_ext_energy - min_ext_energy;
			ext_energy_2_range = max_ext_eng_2 - min_ext_eng_2;
			ext_energy_3_range = max_ext_eng_3 - min_ext_eng_3;

			for(j=0;j<(SQR(RIGHT_CLICK_WINDOW));j++)
			{
				int_eng1 = ((internal_energy_1[j]-min_energy1)/energy1_range)*AlgoOpt->right_IE1_W;
				int_eng2 = ((internal_energy_2[j]-min_energy2)/energy2_range)*AlgoOpt->right_IE2_W;
				ext_eng = ((external_energy[j]-min_ext_energy)/ext_energy_range)*AlgoOpt->right_EE1_W;
				ext_energy_2 = ((external_energy_2[j]-min_ext_eng_2)/ext_energy_2_range)*AlgoOpt->right_EE2_W;
				ext_energy_3 = ((external_energy_3[j]-min_ext_eng_3)/ext_energy_3_range)*AlgoOpt->right_EE3_W;

				total_eng = int_eng1+int_eng2+ext_energy_2-ext_eng+ext_energy_3;
				total_energy[j] = total_eng;
				if(!j || total_eng < min_total_energy)
				{
					min_total_energy = total_eng;
					count = j;/*location of lowest total energy in window*/
				}
			}

			/*udpate the new contour position to the lowest total energy location in 7x7 window*/
			ret = convert_index2height_width(count,RIGHT_CLICK_WINDOW,RIGHT_CLICK_WINDOW,&next_xpos,&next_ypos);
			if(ret != 0 || 0 > next_xpos || 0>next_ypos )
				continue;
			curr_cont->last_x_pos = curr_cont->x_pos;
			curr_cont->last_y_pos = curr_cont->y_pos;

			if(next_xpos>(RIGHT_CLICK_WINDOW/2))
				curr_cont->x_pos = curr_cont->x_pos + (next_xpos-(RIGHT_CLICK_WINDOW/2));
			else if(next_xpos<(RIGHT_CLICK_WINDOW/2))
				curr_cont->x_pos = curr_cont->x_pos - (next_xpos+(RIGHT_CLICK_WINDOW/2));

			if(next_ypos>(RIGHT_CLICK_WINDOW/2))
				curr_cont->y_pos = curr_cont->y_pos + (next_ypos-(RIGHT_CLICK_WINDOW/2));
			else if(next_ypos<(RIGHT_CLICK_WINDOW/2))
				curr_cont->y_pos = curr_cont->y_pos + (next_ypos-(RIGHT_CLICK_WINDOW/2));

			ret = convert_height_width2index(&curr_cont->index,ROW,COL,curr_cont->x_pos,curr_cont->y_pos);
			if(ret != 0 || 0 > curr_cont->index )
				continue;
		}
	}

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

	if(external_energy_2)
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

void AlterActiveContourAlgo(unsigned char *image,unsigned char *sobel_image,
								  unsigned char *rgb_image,contour_position* contour_head,int ROW,int COL,Algo_options* AlgoOpt)
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
	float min_total_energy = 0,energy1_range = 0,energy2_range = 0,ext_energy_range = 0,ext_energy_2_range = 0;

	float *external_energy_2 = NULL,max_ext_eng_2 = 0,min_ext_eng_2 = 0,ext_energy_2 = 0;/*To find the image intensity variance*/
	float intensity_sum = 0,intensity_mean = 0,variance = 0;
	float red_avg = 0,green_avg =0,blue_avg =0;
	float centroid_xpos = 0,centroid_ypos = 0;

	int ret = 0;

	int ALTER_CLICK_WINDOW = AlgoOpt->alter_click_window;


	internal_energy_1 = (float*)calloc(ALTER_CLICK_WINDOW*ALTER_CLICK_WINDOW,sizeof(float));
	if(!internal_energy_1)
	{
		printf("memory allocation failed\n");
		return;
	}

	internal_energy_2 = (float*)calloc(ALTER_CLICK_WINDOW*ALTER_CLICK_WINDOW,sizeof(float));
	if(!internal_energy_2)
	{
		printf("memory allocation failed\n");
		return;
	}

	external_energy = (float*)calloc(ALTER_CLICK_WINDOW*ALTER_CLICK_WINDOW,sizeof(float));
	if(!external_energy)
	{
		printf("memory allocation failed\n");
		return;
	}

	external_energy_2 = (float*)calloc(ALTER_CLICK_WINDOW*ALTER_CLICK_WINDOW,sizeof(float));
	if(!external_energy_2)
	{
		printf("memory allocation failed\n");
		return;
	}

	total_energy = (float*)calloc(ALTER_CLICK_WINDOW*ALTER_CLICK_WINDOW,sizeof(float));
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

	for(i=0;i<1;i++)
	{
		contour_head_xpos = contour_head->x_pos;
		contour_head_ypos = contour_head->y_pos;

		/*To calculate the average distance between the contour points*/
		CalculateAverageContourDistance(contour_head,&avrg_contour_distance);

		/*To claculate the centroid of the countour points*/
		CalculateContourCentroid(contour_head,&centroid_xpos,&centroid_ypos);
		ret = convert_height_width2rgbindex(&index,ROW,COL,centroid_xpos,centroid_ypos);
		if(ret != 0 || 0 > index )
			continue;

		red_avg = rgb_image[index];
		green_avg = rgb_image[index+1];
		blue_avg = rgb_image[index+2];

		/*To calculate the internal energies and
		external energy at each contour point around 7x7 pixel window*/
		curr_cont = next_cont = contour_head;
		while(next_cont)
		{	

			curr_cont = next_cont;
			next_cont = curr_cont->next;

			cur_xpos = curr_cont->x_pos;
			cur_ypos = curr_cont->y_pos;

			 if(curr_cont->pos_state == eFixed)
				 continue;

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
			for(r1=-(ALTER_CLICK_WINDOW/2);r1<=(ALTER_CLICK_WINDOW/2);r1++)
			{
				for(c1=-(ALTER_CLICK_WINDOW/2);c1<=(ALTER_CLICK_WINDOW/2);c1++)
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
					ret = convert_height_width2index(&index,ROW,COL,cur_xpos+c1,cur_ypos+r1);
					if(ret != 0 || 0 > index )
						continue;

					sobel_pixel_square = SQR(sobel_image[index]);
					external_energy[count] = sobel_pixel_square;

					/*External Energy 2
					(Variance of the rgb image intensity from the rgb image intensity of the centroid)*/
					ret = convert_height_width2rgbindex(&index,ROW,COL,cur_xpos+c1,cur_ypos+r1);
					if(ret != 0 || 0 > index )
						continue;

					variance = SQR(abs(red_avg-rgb_image[index])+\
						abs(green_avg-rgb_image[index+1])+abs(blue_avg-rgb_image[index+2]));
					external_energy_2[count] = variance;
					if(!count || variance < min_ext_eng_2)
						min_ext_eng_2 = variance;

					if(!count|| variance > max_ext_eng_2)
						max_ext_eng_2 = variance;

					count ++;
				}
			}

			count = 0;min_total_energy = 0;
			ext_eng =0;ext_energy_2 = 0;int_eng1 = 0;int_eng2 = 0;total_eng = 0;

			/*Normalise the individual energies and find total energy*/
			energy1_range	  = max_energy1 - min_energy1;
			energy2_range	  = max_energy2 - min_energy2;  
			ext_energy_range  = max_ext_energy - min_ext_energy;
			ext_energy_2_range = max_ext_eng_2 - min_ext_eng_2;

			for(j=0;j<(SQR(ALTER_CLICK_WINDOW));j++)
			{
				int_eng1 = ((internal_energy_1[j]-min_energy1)/energy1_range)*AlgoOpt->alter_IE1_W;
				int_eng2 = ((internal_energy_2[j]-min_energy2)/energy2_range)*AlgoOpt->alter_IE2_W;
				ext_eng = ((external_energy[j]-min_ext_energy)/ext_energy_range)*AlgoOpt->alter_EE1_W;
				ext_energy_2 = ((external_energy_2[j]-min_ext_eng_2)/ext_energy_2_range)*AlgoOpt->alter_EE2_W;

				total_eng = int_eng1+int_eng2-ext_eng+ext_energy_2;
				total_energy[j] = total_eng;
				if(!j || total_eng < min_total_energy)
				{
					min_total_energy = total_eng;
					count = j;/*location of lowest total energy in window*/
				}
			}

			/*udpate the new contour position to the lowest total energy location in 7x7 window*/
			ret = convert_index2height_width(count,ALTER_CLICK_WINDOW,ALTER_CLICK_WINDOW,&next_xpos,&next_ypos);
			if(ret != 0 || 0 > next_xpos || 0 > next_ypos )
				continue;

			curr_cont->last_x_pos = curr_cont->x_pos;
			curr_cont->last_y_pos = curr_cont->y_pos;

			if(next_xpos>(ALTER_CLICK_WINDOW/2))
				curr_cont->x_pos = curr_cont->x_pos + (next_xpos-(ALTER_CLICK_WINDOW/2));
			else if(next_xpos<(ALTER_CLICK_WINDOW/2))
				curr_cont->x_pos = curr_cont->x_pos - (next_xpos+(ALTER_CLICK_WINDOW/2));

			if(next_ypos>(ALTER_CLICK_WINDOW/2))
				curr_cont->y_pos = curr_cont->y_pos + (next_ypos-(ALTER_CLICK_WINDOW/2));
			else if(next_ypos<(ALTER_CLICK_WINDOW/2))
				curr_cont->y_pos = curr_cont->y_pos + (next_ypos-(ALTER_CLICK_WINDOW/2));

			ret = convert_height_width2index(&curr_cont->index,ROW,COL,curr_cont->x_pos,curr_cont->y_pos);
			if(ret != 0 || 0 > curr_cont->index )
				continue;

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

	if(external_energy_2)
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


