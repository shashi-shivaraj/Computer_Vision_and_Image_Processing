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
				double **norms,	/*3D norms of the image*/
				int Orientation_Predicate)		

{
	int	r2,c2;
	int	queue[MAX_QUEUE],qh,qt;
	double	average,total;	/* average and total orinetation in growing region */
	int pixel_x_pos = 0,pixel_y_pos = 0,index;
	double seed_norm[3] = {0,0,0};
	double pixel_norm[3] = {0,0,0};

	double dot_ab=0,mag_a=0,mag_b=0,cos_theta,theta;
	int x_pos = 0,y_pos = 0;


	*count=0;
	if (labels[r*COLS+c] != paint_over_label)
		return;
	labels[r*COLS+c]=new_label;
	//average=total=(int)image[r*COLS+c];
	average=total=0;

	convert_height_width2index(&index,ROWS,COLS,c,r);
	/*Seed value*/
	seed_norm[0] = norms[0][index];
	seed_norm[1] = norms[1][index];
	seed_norm[2] = norms[2][index];

	if (indices != NULL)
		indices[0]=r*COLS+c;
	queue[0]=r*COLS+c;
	qh=1;	/* queue head */
	qt=0;	/* queue tail */
	(*count)=1;
	while (qt != qh)
	{
		/* recalculate average after each pixels join */
			if(*count != 1 )
			average=total/(*count - 1);

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
				if(image[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2] == 0)
					continue;

				
				/* Orientation test criterias to join region */
				/*Pixel norm value*/
				pixel_norm[0] = norms[0][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2];
				pixel_norm[1] = norms[1][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2];
				pixel_norm[2] = norms[2][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2];

				dot_ab=seed_norm[0]*pixel_norm[0]+seed_norm[1]*pixel_norm[1]+seed_norm[2]*pixel_norm[2];
				mag_a=sqrt(SQR(seed_norm[0])+SQR(seed_norm[1])+SQR(seed_norm[2]));
				mag_b=sqrt(SQR(pixel_norm[0])+SQR(pixel_norm[1])+SQR(pixel_norm[2]));
				cos_theta = dot_ab/(mag_a*mag_b);
				theta = CONV_2_DEGREE(acos(cos_theta));
			//	printf("angle in degree %d\n",theta);

				if (abs(theta-average) > Orientation_Predicate)
					continue;
				

				labels[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]=new_label;
				if (indices != NULL)
					indices[*count]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;
				total = total + theta;

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

void odetics2coords(unsigned char *RangeImage,int ROWS,int COLS,double **P,int ImageTypeFlag)
{

	int	r,c;
	double	cp[7];
	double 	xangle,yangle,dist;
	double 	ScanDirectionFlag,SlantCorrection;

	cp[0]=1220.7;		/* horizontal mirror angular velocity in rpm */
	cp[1]=32.0;		/* scan time per single pixel in microseconds */
	cp[2]=(COLS/2)-0.5;		/* middle value of columns */
	cp[3]=1220.7/192.0;	/* vertical mirror angular velocity in rpm */
	cp[4]=6.14;		/* scan time (with retrace) per line in milliseconds */
	cp[5]=(ROWS/2)-0.5;		/* middle value of rows */
	cp[6]=10.0;		/* standoff distance in range units (3.66cm per r.u.) */

	cp[0]=cp[0]*3.1415927/30.0;	/* convert rpm to rad/sec */
	cp[3]=cp[3]*3.1415927/30.0;	/* convert rpm to rad/sec */
	cp[0]=2.0*cp[0];		/* beam ang. vel. is twice mirror ang. vel. */
	cp[3]=2.0*cp[3];		/* beam ang. vel. is twice mirror ang. vel. */
	cp[1]/=1000000.0;		/* units are microseconds : 10^-6 */
	cp[4]/=1000.0;			/* units are milliseconds : 10^-3 */

	switch(ImageTypeFlag)
	{
	case 1:		/* Odetics image -- scan direction upward */
		ScanDirectionFlag=-1;
		break;
	case 0:		/* Odetics image -- scan direction downward */
		ScanDirectionFlag=1;
		break;
	default:		/* in case we want to do this on synthetic model */
		ScanDirectionFlag=0;
		break;
	}

	/* start with semi-spherical coordinates from laser-range-finder: */
	/*			(r,c,RangeImage[r*COLS+c])		  */
	/* convert those to axis-independant spherical coordinates:	  */
	/*			(xangle,yangle,dist)			  */
	/* then convert the spherical coordinates to cartesian:           */
	/*			(P => X[] Y[] Z[])			  */

	if (ImageTypeFlag != 3)
	{
		for (r=0; r<ROWS; r++)
		{
			for (c=0; c<COLS; c++)
			{
				SlantCorrection=cp[3]*cp[1]*((double )c-cp[2]);
				xangle=cp[0]*cp[1]*((double )c-cp[2]);
				yangle=(cp[3]*cp[4]*(cp[5]-(double )r))+	/* Standard Transform Part */
					SlantCorrection*ScanDirectionFlag;	/*  + slant correction */
				dist=(double )RangeImage[r*COLS+c]+cp[6];
				P[2][r*COLS+c]=sqrt((dist*dist)/(1.0+(tan(xangle)*tan(xangle))
					+(tan(yangle)*tan(yangle))));
				P[0][r*COLS+c]=tan(xangle)*P[2][r*COLS+c];
				P[1][r*COLS+c]=tan(yangle)*P[2][r*COLS+c];
			}
		}
	}
}