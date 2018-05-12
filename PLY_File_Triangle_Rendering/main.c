/**********************************************************************
*
*  FILE NAME	: main.c
*
*  DESCRIPTION  : Implement triangle rendering for a PLY - Polygon File Format.
* 
*  PLATFORM		: Win32
*
*  DATE                 NAME                     REASON
*  27th Feb,2018        Shashi Shivaraju         ECE_6680_Lab_05
*                       [C88650674]
***********************************************************************/

/*Header file inclusions*/
#include "header.h"

int main(int argc, char *argv[])/*main function of the program*/
{
	FILE* fp = NULL;	/*file pointer to access the file*/
	char filename[MAX_FILENAME] = {0};	/*character array to store the filename*/
	unsigned int file_size = 0;	/*variable to store file size in bytes*/
	float X_Degree = 0,Y_Degree = 0,Z_Degree = 0;
	int ret =0;
	char option = 0;

 	while(1)
	{

printf("*************************************\n\
**************Triangle Rendering*****\n\
*************************************\n");
printf("Select usage\n\n\
Enter 1 to process a PLY file\n\
Enter 0 to Exit the program\n");

		scanf(" %c",&option);

		if('1' == option)
		{
			printf("\nEnter the filename\n");
			scanf("%s",filename);

			fp = fopen(filename,"r");
			if(!fp)
			{
				printf("fopen failed for %s",filename);
				return -1;
			}

			//fseek(fp, 0L, SEEK_END);
			//file_size = ftell(fp);
			//fseek(fp, 0L, SEEK_SET);
			//printf("\nThe PLY file size is %d\n",file_size);

			printf("\nEnter the rotation angles [X_Degree] [Y_Degree] [Z_Degree]\n");
			scanf("%f %f %f",&X_Degree,&Y_Degree,&Z_Degree);

			if(0 > CHECK_ANGLE(X_Degree) ||
			   0 > CHECK_ANGLE(Y_Degree)||
			   0 > CHECK_ANGLE(Z_Degree) )
			{
				printf("invalid angle entered!!!! Try Again");
				option = '2'; /*setting it to invalid option to trigger retry*/
			}	
		}

		switch(option)
		{
		case '1':
			{
				RenderTriangles(fp,X_Degree,Y_Degree,Z_Degree);
			}
			break;
		case '0':
			{
				printf("\nexiting the program\n");
			}
			break;
		default:
			{
				printf("\ninvalid option: %c;Retry\n",option);
				break;
			}
		}

		if(fp)
		{
			fclose(fp);
			fp = NULL;
		}

		if('0' == option)
		{
			break;
		}
	}

	Sleep(1000);

	return 0;
}