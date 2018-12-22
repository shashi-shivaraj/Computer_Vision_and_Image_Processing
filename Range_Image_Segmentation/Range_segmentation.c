#include"header.h"

int main(int argc,char*argv[])
{
	FILE* fp = NULL;                /*File pointer for file operations*/
	unsigned char *image = NULL,
				  *seg_image = NULL,
				  *labels = NULL;
	double 		  *coords[3] = {0,0,0};
	double  		  *norms[3]	 = {0,0,0};
	int			  *indices  = NULL;
	double  		x[3] = {0,0,0},
					a[3] = {0,0,0},
					b[3] = {0,0,0},
					bx[3] = {0,0,0},
					ax[3] = {0,0,0};
					
	char magic[10];				/*char pointer to read the header in the data file*/

	int ROW=0,COL=0,max_pixel = 0,RegionSize = 0;
	int i =0,j = 0,count = 0,ret=0;
	int ypos = 0,xpos = 0,index = 0;
	int r1=0,c1=0,C=0,R=0;
	int seedpixel_flag = E_TRUE;
	int	prev_grayscale = 0;
	
	if(argc != 2)
	{
		printf("[Usage]./exe range_image.ppm\n");
		return -1;
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

	//printf("Header info of the opened PPM file is max pixel = %d,\
		   col = %d,row = %d, magic code = %s\n",max_pixel,COL,ROW,magic);/*Display header info of image*/

	image = (unsigned char *)calloc(ROW*COL,sizeof(unsigned char));/*Allocate memory to store input image data*/
	seg_image = (unsigned char *)calloc(ROW*COL,sizeof(unsigned char));
	if(!image || !seg_image)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
	}

	labels=(unsigned char *)calloc(ROW*COL,sizeof(unsigned char));
	if(!labels)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
	}

	/* used to paint the grown regions */
	indices=(int *)calloc(ROW*COL,sizeof(int));
	if(!indices)			/*Error handling*/
	{
		printf("calloc failed\n");/*Memory allocation failed*/
		return -1;              /*return error code*/
	}


	/*Allocate for 3D Coordinates*/
	for(i=0;i<3;i++)
	{
		coords[i] = (double  *)calloc(ROW*COL,sizeof(double ));
		norms[i] = (double  *)calloc(ROW*COL,sizeof(double ));
	}


	ret = fread(image,1,ROW*COL,fp);/*read the image data form file and store in a buffer*/
	if(ret != ROW*COL)              /*check for invalid fread*/
	{
		printf("fread failed to read %d data from file",ROW*COL); /*fread operation failed*/
		return -1;              /*return error code*/
	}

	if(fp)
		fclose(fp);
	fp = NULL;

	/*Threshold the image to remove background*/
	for(i=0;i<ROW*COL;i++)
	{
		if(INTENSITY_THRESHOLD < image[i])
		{
			image[i] = 0;
		}
		seg_image[i]=image[i];
	}


	fp = fopen("thresholded_image.ppm","wb+");/*open output image file*/
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

	/*Convert pixels into 3D coordinates*/
	odetics2coords(image,ROW,COL,coords,1);


	fp = fopen("3D_Coords.txt","w+");/*open output image file*/
	if(!fp)                     /*error handling*/
	{
		printf("fopen failed for %s\n","intial_contours.ppm");/*failure to open the output file*/
		return -1;          /*return error code*/
	}

	for(i=0;i<ROW*COL;i++)
	{
		fprintf(fp,"x:%f y:%f z:%f\n",coords[0][i],coords[1][i],coords[2][i]);
	}

	if(fp)                      /*Close the output file handle*/
	{
		fclose(fp);
		fp = NULL;
	}

	count = 0;
	/*calculate the surface normals for the image */
	for(i=0;i<ROW*COL;i++)
	{
		/*position of pixel i in image*/
		ret = convert_index2height_width(i,ROW,COL,&xpos,&ypos);
		if(ret != 0)
		{
			printf("invalid index %d",i);
			continue;
		}
		else
		{
			//printf("pixel index=%d xpos %d ypos %d\n",i,xpos,ypos);
		}

		x[0]=coords[0][i];
		x[1]=coords[1][i];
		x[2]=coords[2][i];

		ret = convert_height_width2index(&index,ROW,COL,xpos+NORM_DIST,ypos);
		if(ret != 0)
		{
			//printf("border pixel index=%d xpos %d ypos %d\n",i,xpos+NORM_DIST,ypos);
			count++;
			norms[0][i] = norms[0][i-1];
			norms[1][i] = norms[1][i-1];
			norms[2][i] = norms[2][i-1];
			//image[index] = 0;
			continue;
		}

		a[0]=coords[0][index];
		a[1]=coords[1][index];
		a[2]=coords[2][index];

		ret = convert_height_width2index(&index,ROW,COL,xpos,ypos+NORM_DIST);
		if(ret != 0)
		{
			//printf("border pixel index=%d xpos %d ypos %d\n",i,xpos,ypos+NORM_DIST);
			count++;
			norms[0][i] = norms[0][i-1];
			norms[1][i] = norms[1][i-1];
			norms[2][i] = norms[2][i-1];
			//image[index] = 0;
			continue;
		}

		b[0]=coords[0][index];
		b[1]=coords[1][index];
		b[2]=coords[2][index];

		for(j=0;j<3;j++)
		{
			bx[j]=b[j]-x[j];
			ax[j]=a[j]-x[j];
		}

		/*norm(x) = cross product (b-x)*(x-a)*/
		norms[0][i] = bx[1]*ax[2]-ax[1]*bx[2];
		norms[1][i] = bx[2]*ax[0]-ax[2]*bx[0];
		norms[2][i] = bx[0]*ax[1]-ax[0]*bx[1];

	}

	//printf("total unprocessed pixel count %d\n",count);

	fp = fopen("3D_Norms.txt","w+");/*open output image file*/
	if(!fp)                     /*error handling*/
	{
		printf("fopen failed for %s\n","intial_contours.ppm");/*failure to open the output file*/
		return -1;          /*return error code*/
	}

	for(i=0;i<ROW*COL;i++)
	{
		fprintf(fp,"x:%f y:%f z:%f\n",norms[0][i],norms[1][i],norms[2][i]);
	}

	if(fp)                      /*Close the output file handle*/
	{
		fclose(fp);
		fp = NULL;
	}

	fp = fopen("Regions.txt","w+");/*open output image file*/
	if(!fp)                     /*error handling*/
	{
		printf("fopen failed for %s\n","intial_contours.ppm");/*failure to open the output file*/
		return -1;          /*return error code*/
	}


	count = 0;
	for(R=2;R<ROW-2;R++)          
	{
		for(C=2;C<COL-2;C++)
		{
			seedpixel_flag = E_TRUE;	/*Assume its a seed pixel*/
			/*Check 5x5 window to ensure it is a valid seed pixel*/
			for(r1=-2;r1<=2;r1++)
			{
				for(c1=-2;c1<=2;c1++)
				{
					if(0 == image[(R+r1)*COL+(C+c1)]||
						0 != labels[(R+r1)*COL+(C+c1)])
					{
						seedpixel_flag = E_FALSE;
						break;
					}
				}
				if(seedpixel_flag == E_FALSE)
					break;
			}

			if(seedpixel_flag == E_FALSE)
					continue;
			
			count++;

			/*Use region growing with the seed pixel*/
			RegionGrow(image,labels,ROW,COL,R,C,0,count,
		indices,&RegionSize,norms,ORIENTATION_THRESHOLD);
	
		
			///*do not paint if the region size is very small*/
			//if(RegionSize < MIN_REGION_SIZE)
			//{
			//	/*for (i=0; i<RegionSize; i++)
			//	{
			//		labels[indices[i]]=0;
			//	}
			//	count --;*/
			//	continue;
			//}


				x[0] = 0;
				x[1] = 0;
				x[2] = 0;
			/*paint the grown region with gray shades*/
				for (i=0; i<RegionSize; i++)
				{
					seg_image[indices[i]]= GREY_SHADE+(count-1)*30;
					x[0] = x[0]+norms[0][indices[i]];
					x[1] = x[1]+norms[1][indices[i]];
					x[2] = x[2]+norms[2][indices[i]];
				}

				
				fprintf(fp,"Region lable:%d Region Size:%d GreyScale Colour %d,Avg Surface normal for region: x:%.6f y:%.6f z:%.6f\n",count,RegionSize,GREY_SHADE+(count-1)*30,
						   (x[0]/RegionSize),(x[1]/RegionSize),(x[2]/RegionSize));

			/*clear the indices for next region grow*/
				for(i=0;i<ROW*COL;i++)
				{
					indices[i] = 0;
				}
		}
	}

	/*paint the pixels into region*/
	for(i=0;i<ROW*COL;i++)
	{
		switch(seg_image[i])
		{
			case 0:
			case 40:
			case 70:
			case 100:
			case 130:
			case 160:
			case 190:
			case 220:
				{
					break;		
				}
			default:
				{	/*not inside a region*/
					convert_index2height_width(i,ROW,COL,&xpos,&ypos);
					convert_height_width2index(&index,ROW,COL,xpos-NORM_DIST,ypos);
					seg_image[i] = seg_image[index];
				}
				break;
		}
	}


	
	if(fp)                      /*Close the output file handle*/
	{
		fclose(fp);
		fp = NULL;
	}

	fp = fopen("segmented_range_image.ppm","wb+");/*open output image file*/
	if(!fp)                     /*error handling*/
	{
		printf("fopen failed for %s\n","segmented_range_image.ppm");/*failure to open the output file*/
		return -1;          /*return error code*/
	}

	fprintf(fp,"P5 %d %d 255 ",COL,ROW);/*Write the header as per PPM image specification to output image file*/
	fwrite(seg_image,1,ROW*COL,fp);/*write the output image data into file*/

	if(fp)                      /*Close the output file handle*/
	{
		fclose(fp);
		fp = NULL;
	}

	/*resource deallocation*/
	if(fp)
		fclose(fp);
	fp = NULL;

	if(image)
		free(image);
	image = NULL;

	return 0;
}
