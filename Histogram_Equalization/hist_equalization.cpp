#include <iostream>
#include <cstdio>
#include <cstring>

int main(int argc,char* argv[])
{
	
	FILE* fp = NULL,*fp_image= NULL;
	int row=0,col=0,max_pixel=0,ret=0;
	char magic[10];
	int Hist[256] = {0};
	int i=0,j=0;
	double nhist[256],chist[256];
	
	unsigned char *image = NULL;
		
	if(2 != argc)
	{
		printf("usage is ./equalize.out file \n");
		return -1;
	}
			
	fp = fopen(argv[1],"r");
	if(!fp)
	{
		printf("fopen failed for %s\n",argv[1]);
		return -1;
	}
	
	fp_image = fopen("equalized.ppm","w+");
	if(!fp_image)
	{
		printf("fopen failed for %s\n","brighter.ppm");
		return -1;
	}
	
	ret = fscanf(fp,"%s %d %d %d ",magic,&col,&row,&max_pixel);
	if(4 != ret || 255 != max_pixel || 0 != strcmp("P5",magic)) /*specific to 8bit greyscale PPM image*/
	{
		printf("Not a greyscale image of PPM format\n");
		return -1;
	}
	
//	printf("Header info of the opened PPM file is max pixel = %d,\
col = %d,row = %d, magic code = %s\n",max_pixel,col,row,magic);

	image = (unsigned char *)calloc(row*col,sizeof(unsigned char));
	if(!image)
	{
		printf("calloc failed\n");
		return -1;
	}
	
	ret = fread(image,1,row*col,fp);
	if(ret != row*col)
	{
		printf("fread failed to read %d data from file",row*col);
		return -1;
	}
	
	/*
	 * Connot use memset because 
	 * memset sets bytes and works for characters 
	 * because they are single bytes, but integers are not
	 * 
	 */
	 
	for(i=0;i<256;i++)
	{
		Hist[i] = 0;
	}
	
	for(i=0;i<row*col;i++)
	{
		Hist[image[i]]++; /*Histogram of original image*/
	}
	
	//printf(" Original Histogram values are:\n");
	//for(i=0;i<256;i++)
	//{
		//printf("%d\n",Hist[i]);
	//} 
	
	for(i=0;i<256;i++)
	{
		nhist[i]=(double)Hist[i]/(double)(row*col); /*Normalize the Histogram */
	}
	
	/*printf("Normalized Histogram values are:\n");
	for(i=0;i<256;i++)
	{
		printf("%f\n",nhist[i]);
	}*/
	
	chist[0] = nhist[0];
	for (i=1; i<256; i++)
	{							
		chist[i]=chist[i-1]+nhist[i]; /* Normalized Cumulative Distribution */
	}
	
	/*printf("Cumulative Histogram values are:\n");
	for(i=0;i<256;i++)
	{
		printf("%f\n",chist[i]);
	}*/
	
	for (i=0; i<row*col; i++)
	{
		/* remap pixels according to chist */
		/*
		 * scale the Normalized Cumulative Distribution value correspond1ing
		 * to image[i] = pixel value of original
		 * 
		 */
		image[i]=(unsigned char)(255.0 * chist[image[i]]); 
	}
	
	/*Write the header as per PPM image specification*/
	fprintf(fp_image,"P5 %d %d 255 ",col,row);
	fwrite(image,1,row*col,fp_image);
	
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
	
	if(fp_image)
	{
		fclose(fp_image);
		fp_image = NULL;
	}
	
	if(image)
	{
		free(image);
		image = NULL;
	}
	
	return 0;
}
