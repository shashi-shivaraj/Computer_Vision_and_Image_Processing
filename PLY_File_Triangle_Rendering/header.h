/**********************************************************************
*
*  FILE NAME	: header.h
*
*  DESCRIPTION  : Header file
* 
*  PLATFORM		: Win32
*
*  DATE                 NAME                    REASON
*  27th Feb,2018        ShashiShivaraju         ECE_6680_Lab_05
*                       [C88650674]
***********************************************************************/

/*Header file inclusions*/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>	/*included for strcmp() and memset() api*/
#include <windows.h>
#include <math.h>

/*Macro definations*/
#define MAX_8BIT	255
#define MIN_8BIT	0		
#define MAX_FILENAME	500

#define PI_VALUE				3.14159265358979323846
#define DEFAULT_ZBUFFER_DEPTH	999999
#define d_THRESHOLD				0.0000000005

#define CHECK_ANGLE(X) ((X>360 || X <-360)? -1:0)
#define SQR(X)	X*X

/*declarations of datatypes*/
typedef struct __vertex_data
{
	unsigned int index;
	float x_pos;
	float y_pos;
	float z_pos;
}vertex_data,vector;

/*declarations of datatypes*/
typedef struct __face_data
{
	unsigned int index;
	unsigned int total_vertices; /*Always 3 in triangle rendering*/
	unsigned int vertex_one;
	unsigned int vertex_two;
	unsigned int vertex_three;
}face_data;


/*Function Prototypes*/

/*Function to render triangles based on PLY file and camera roation angles*/
void RenderTriangles(FILE * PLY_fpt,float X_Degree,float Y_Degree,float Z_Degree);

/* Function to parse the PLY header to obtain number of vertices and number of faces*/
void ParsePLYHeader(FILE * PLY_fpt,int *vertices,int *faces);

/* Function to read the vertices and faces from the PLY file */
void ReadPLY_Vertices_Faces(FILE * PLY_fpt,
							unsigned int total_vertices,vertex_data** vertex_pointers,
							unsigned int total_faces,face_data** face_pointers,
							vertex_data *min_vertex,vertex_data *max_vertex);

/* Function to initialize the rotation matrices*/
void initialize_rotation_matrices(float rotation_x[3][3],float rotation_y[3][3],float rotation_z[3][3],
								  float X_Degree,float Y_Degree,float Z_Degree);

/*Function to multiply vector with a 3x3 rotation matrix*/
void matrix_multiply(vector *cam_pos,float rotation[3][3],vector *out);

/*Function to calculate cross product of two vectors*/
void cross_product(vector *u,vector *v,vector *uxv);

/*Function to calculate scalar product of two vectors*/
float scalar_product(vector *u,vector *v);

/*calculate the 3D coordinates bounding the image*/
void calculate_3D_bounding_box_coordinates(vector *cam_pos,vertex_data *center_vertex,vector *up_pos,
									 vector *left_pos,vector *right_pos,vector *top_pos,
									 vector *bottom_pos,vector *top_left,vector *temp,
									 float *a,float E);

/*Function to convert pixel index to (x,y) coordinates of the image */
int convert_index2height_width(int index,
								int ROW,int COL,/* size of image */
								int *x_pos,int *y_pos);/* pixel position */