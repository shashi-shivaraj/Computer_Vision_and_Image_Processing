/*header file inclusion*/
#include "header.h"
/*macro declaration*/
#define MAX_QUEUE 10000	/* max perimeter size (pixels) of border wavefront */

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
				int *count,	/* output:  count of pixels painted */
				int Intensity_Predicate,
				int Centroid_Predicate)		

{
	int	r2,c2;
	int	queue[MAX_QUEUE],qh,qt;
	int	average,total;	/* average and total intensity in growing region */
	double centroid_x_pos = 0,centroid_y_pos =0,dist_from_centroid = 0;
	int x_pos_sum = 0,y_pos_sum = 0,pixel_x_pos = 0,pixel_y_pos = 0;

	*count=0;
	if (labels[r*COLS+c] != paint_over_label)
		return;
	labels[r*COLS+c]=new_label;
	average=total=(int)image[r*COLS+c];
	
	/*initial value*/
	centroid_x_pos = x_pos_sum = c;
	centroid_y_pos = y_pos_sum = r;

	if (indices != NULL)
		indices[0]=r*COLS+c;
	queue[0]=r*COLS+c;
	qh=1;	/* queue head */
	qt=0;	/* queue tail */
	(*count)=1;
	while (qt != qh)
	{
		if ((*count)%50 == 0)	/* recalculate average and centroid after each 50 pixels join */
		{
			average=total/(*count);
			
			centroid_x_pos = x_pos_sum/(*count);
			centroid_y_pos = y_pos_sum/(*count);
		}
		for (r2=-1; r2<=1; r2++)
			for (c2=-1; c2<=1; c2++)
			{
				if (r2 == 0  &&  c2 == 0)
					continue;
				if ((queue[qt]/COLS+r2) < 0  ||  (queue[qt]/COLS+r2) >= ROWS  ||
					(queue[qt]%COLS+c2) < 0  ||  (queue[qt]%COLS+c2) >= COLS)
					continue;
				if (labels[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]!=paint_over_label)
					continue;
				/* Intensity test criterias to join region */
				if (abs((int)(image[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2])
					-average) > Intensity_Predicate)
					continue;

				convert_index2height_width((queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2
											,ROWS,COLS,&pixel_x_pos,&pixel_y_pos);
				/* Centroid test criterias to join region */
				dist_from_centroid = sqrt((SQR(centroid_x_pos - pixel_x_pos)+\
					SQR(centroid_y_pos - pixel_y_pos))); 
				if(dist_from_centroid > Centroid_Predicate)
					continue;

				labels[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]=new_label;
				if (indices != NULL)
					indices[*count]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;
				total+=image[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2];

				/*To calculate the centroid*/
				x_pos_sum = x_pos_sum + pixel_x_pos;
				y_pos_sum = y_pos_sum + pixel_y_pos;

				(*count)++;
				queue[qh]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;
				qh=(qh+1)%MAX_QUEUE;
				if (qh == qt)
				{
					printf("Max queue size exceeded\n");
					exit(0);
				}
			}
			qt=(qt+1)%MAX_QUEUE;
	}
}