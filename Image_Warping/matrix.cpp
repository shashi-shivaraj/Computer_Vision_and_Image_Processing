/**************************************************************************
*  FILE NAME	: matrix.cpp
*
*  DESCRIPTION  : Matrix manipulation routines.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  27th Oct,2018        Shashi Shivaraju        CPSC_6040_HW_05
*                       [C88650674]
* Reference :Ioannis Karamouzas, 10/15/18 & D. House
***************************************************************************/
/*Header file inclusion*/
#include "matrix.h"
#include "manager.h"

Matrix3D::Matrix3D(){
  setidentity();
}

Matrix3D::Matrix3D(const double coefs[3][3]){
  set(coefs);
}

Matrix3D::Matrix3D(const Matrix3D &mat){
  set(mat.M);
}

/*
   Print the contents of a 3x3 transformation matrix
*/
void Matrix3D::print()const{

  printf("\n");
  for(int row = 0; row < 3; row++){
    printf("%8.4f %8.4f %8.4f\n", M[row][0], M[row][1], M[row][2]);
  }
  printf("\n");
}

/*
   Set the matrix m to the indentity matrix
*/
void Matrix3D::setidentity(){

  M[0][0] = M[1][1] = M[2][2] = 1.0;
  M[0][1] = M[0][2] = 0.0;
  M[1][0] = M[1][2] = 0.0;
  M[2][0] = M[2][1] = 0.0;
}

/*
 Set the matrix m to the contents of a 3x3 array
 */
void Matrix3D::set(const double coefs[3][3]){
  
  for(int row = 0; row < 3; row++)
	for(int col = 0; col < 3; col++)
	  M[row][col] = coefs[row][col];
}

/*
   Function to compute and return the determinant of the 3x3 matrix m.
*/
double Matrix3D::determinant()const{
  double det;

  det  = M[0][0] * M[1][1] * M[2][2];
  det += M[0][1] * M[1][2] * M[2][0];
  det += M[0][2] * M[2][1] * M[1][0];
  det -= M[2][0] * M[1][1] * M[0][2];
  det -= M[1][0] * M[0][1] * M[2][2];
  det -= M[0][0] * M[1][2] * M[2][1];

  return det;
}

/*
   Compute the adjoint of 3x3 matrix m and place the result in 3x3
   matrix adj.
*/
Matrix3D Matrix3D::adjoint()const{
  Matrix3D adj;
  
  adj.M[0][0] = (M[1][1] * M[2][2] - M[1][2] * M[2][1]);
  adj.M[0][1] = (M[0][2] * M[2][1] - M[0][1] * M[2][2]);
  adj.M[0][2] = (M[0][1] * M[1][2] - M[0][2] * M[1][1]);
  adj.M[1][0] = (M[1][2] * M[2][0] - M[1][0] * M[2][2]);
  adj.M[1][1] = (M[0][0] * M[2][2] - M[0][2] * M[2][0]);
  adj.M[1][2] = (M[0][2] * M[1][0] - M[0][0] * M[1][2]);
  adj.M[2][0] = (M[1][0] * M[2][1] - M[1][1] * M[2][0]);
  adj.M[2][1] = (M[0][1] * M[2][0] - M[0][0] * M[2][1]);
  adj.M[2][2] = (M[0][0] * M[1][1] - M[0][1] * M[1][0]);
  
  return adj;
}

/*
   Compute the inverse of 3x3 matrix m and place the result in 3x3
   matrix inv.
*/
Matrix3D Matrix3D::inverse()const{
  Matrix3D inv;  
  double det = determinant();

  inv.M[0][0] = (M[1][1] * M[2][2] - M[1][2] * M[2][1]) / det;
  inv.M[0][1] = (M[0][2] * M[2][1] - M[0][1] * M[2][2]) / det;
  inv.M[0][2] = (M[0][1] * M[1][2] - M[0][2] * M[1][1]) / det;
  inv.M[1][0] = (M[1][2] * M[2][0] - M[1][0] * M[2][2]) / det;
  inv.M[1][1] = (M[0][0] * M[2][2] - M[0][2] * M[2][0]) / det;
  inv.M[1][2] = (M[0][2] * M[1][0] - M[0][0] * M[1][2]) / det;
  inv.M[2][0] = (M[1][0] * M[2][1] - M[1][1] * M[2][0]) / det;
  inv.M[2][1] = (M[0][1] * M[2][0] - M[0][0] * M[2][1]) / det;
  inv.M[2][2] = (M[0][0] * M[1][1] - M[0][1] * M[1][0]) / det;
  
  return inv;
}

/*
 Multiply the 3x3 matrix m by 2D vector v.  The process is to
 1) extend v by adding a z coordinate of 1.0,
 2) multiply m by this 3D vector
*/
Vector3D Matrix3D::operator*(const Vector2D &v)const{
  Vector3D vout;
  
  vout.x =  M[0][0] * v.x + M[0][1] * v.y + M[0][2];
  vout.y =  M[1][0] * v.x + M[1][1] * v.y + M[1][2];
  vout.z =  M[2][0] * v.x + M[2][1] * v.y + M[2][2];
  
  return vout;
}

/*
 Multiply the 3x3 matrix m by the 3D vector v
 */
Vector3D Matrix3D::operator*(const Vector3D &v)const{
  Vector3D vout;
  
  vout.x =  M[0][0] * v.x + M[0][1] * v.y + M[0][2] * v.z;
  vout.y =  M[1][0] * v.x + M[1][1] * v.y + M[1][2] * v.z;
  vout.z =  M[2][0] * v.x + M[2][1] * v.y + M[2][2] * v.z;
  
  return vout;
}

/*
 Multiply 3x3 matrix m1 by 3x3 matrix m2 and place the result in
 3x3 matrix m3.  I.e. m3 = m1 * m2
 */
Matrix3D Matrix3D::operator*(const Matrix3D &m2)const{
  double sum;
  int row, col, rc;
  Matrix3D prod;	/* make internal copy in case m3 is m1 or m2 */
  
  for(row = 0; row < 3; row++)
    for(col = 0; col < 3; col++){
      sum = 0.0;
      for(rc = 0; rc < 3; rc++)
		sum += M[row][rc] * m2.M[rc][col];
      prod.M[row][col] = sum;
    }
  
  return prod;
}

double *Matrix3D::operator[](int i){
  return (double *)&(M[i]);
}

void setbilinear(double width, double height, Vector2D xycorners[4],
		 BilinearCoeffs &coeff){

  coeff.width = width;
  coeff.height = height;
  coeff.a0 = xycorners[0].x;
  coeff.b0 = xycorners[0].y;
  coeff.a1 = xycorners[3].x - xycorners[0].x;
  coeff.b1 = xycorners[3].y - xycorners[0].y;
  coeff.a2 = xycorners[1].x - xycorners[0].x;
  coeff.b2 = xycorners[1].y - xycorners[0].y;
  coeff.a3 = xycorners[2].x - xycorners[1].x -
    xycorners[3].x + xycorners[0].x;
  coeff.b3 = xycorners[2].y - xycorners[1].y -
    xycorners[3].y + xycorners[0].y;
  coeff.c2 = coeff.a3 * coeff.b2 - coeff.a2 * coeff.b3;
}

/*
   Compute the inverse bilinear transform of the point xy, back
   into the unit normalized source space uv, using the coefficient block c.
   Then, the uv coordinates unnormalized back into the source image space.
*/
void invbilinear(const BilinearCoeffs &c, Vector2D xy, Vector2D &uv){
  double discriminant;
  double c0, c1;
  static double EPSILON = 1.0e-5;

  if(fabs(c.c2) <= EPSILON){
    uv.y = (c.a1 * xy.y - c.a1 * c.b0 - c.b1 * xy.x + c.a0 * c.b1) /
      (c.a1 * c.b2 - c.a2 * c.b1);
    uv.x = (xy.x - c.a0 - c.a2 * uv.y) / c.a1;
  }
  else{
    c0 = c.a1 * (c.b0 - xy.y) + c.b1 * (xy.x - c.a0);
    c1 = c.a3 * (c.b0 - xy.y) + c.b3 * (xy.x - c.a0) +
      c.a1 * c.b2 - c.a2 * c.b1;
    
    discriminant = sqrt(c1 * c1 - 4.0 * c.c2 * c0);
    
    uv.y =(-c1 + discriminant) / (2.0 * c.c2);
    if(uv.y < 0.0 || uv.y > 1.0){
      uv.y =(-c1 - discriminant) / (2.0 * c.c2);
      uv.x =
	(xy.x - c.a0 - c.a2 * uv.y) / (c.a1 + c.a3 * uv.y);
    }
    else{
      uv.x =
	(xy.x - c.a0 - c.a2 * uv.y) / (c.a1 + c.a3 * uv.y);
      if(uv.x < 0.0 || uv.x > 1.0){
	uv.y =(-c1 - discriminant) / (2.0 * c.c2);
	uv.x =
	  (xy.x - c.a0 - c.a2 * uv.y) / (c.a1 + c.a3 * uv.y);
      }
    }
  }

  uv.x *= c.width;
  uv.y *= c.height;
}


/*
Multiply M by a rotation matrix of angle theta
*/
void Rotate(Matrix3D &M, float theta) 
{
	Matrix3D R; 
	double rad, c, s;
	rad = PI * theta / 180.0;
	c = cos(rad);
	s = sin(rad);
	R[0][0] = c;
	R[0][1] = -s;
	R[1][0] = s;
	R[1][1] = c;

	M = R * M;

}

/*
Multiply M by a scale matrix
*/
void Scale(Matrix3D &M, float scaleX,float scaleY) 
{
	Matrix3D S; 
	
	S[0][0] = scaleX;
	S[1][1] = scaleY;
	M = S * M;
}


/*
Multiply M by a Shear matrix
*/
void Shear(Matrix3D &M, float shearX,float shearY)
{
	Matrix3D SH; 
	
	SH[0][1] = shearX;
	SH[1][0] = shearY;
	
	M = SH * M;
	
}

/*
Multiply M by a Translate matrix
*/
void Translate(Matrix3D &M,float transX,float transY)
{
	Matrix3D T; 
	
	T[0][2] = transX;
	T[1][2] = transY;

	M = T * M;
}

/*
Multiply M by a Perspective matrix
*/
void Perspective(Matrix3D &M,float perspecX,float perspecY)
{
	Matrix3D P;
	
	P[2][0] = perspecX;
	P[2][1] = perspecY;
	M = P * M;
}

/*
Multiply M by a flip matrix
*/
void Flip(Matrix3D &M, int flipX,int flipY)
{ 	
	if(flipX == 1)
	{
		std::cout << "calling horizontal flip"<<std::endl;
		Matrix3D HF;
		HF[0][0] = -1;
		HF[1][1] = 1;
		M = HF * M;
	}
	
	if(flipY == 1)
	{
		std::cout << "calling verticle flip"<<std::endl;
		Matrix3D VF;
		VF[0][0] = 1;
		VF[1][1] = -1;
		M = VF * M;
	}
}

/*
Build a transformation matrix from input text
*/
void read_input(Matrix3D &M,int  &warpMode)
{
	std::string cmd;
	std::cout << "enter 'n' for twirl warp; 'r' for rotate; 's' for scaling; 't' for translation;'h' for shear; 'f' for flipping; 'p' for perspective warp options; 'd' for done\n";
	
	
	warpMode = 0;		/*Perspective wrap mode by default */
	
	/* prompt for user input */
	do
	{
		std::cout << "> ";
		std::cin >> cmd;
		if (cmd.length() != 1){
			std::cout << "invalid command, enter r, s, t, h, f, p, d\n";
		}
		else {
			switch (cmd[0]) {
				case 'b':		/* enable bilinear wrap */
					warpMode = 1;	
					break;
					
				case 'n':		/*enable twirl warp*/
					warpMode = 2;
					std::cout << "twirl warp selected;press d next"<<std::endl;	
					break;
					
				case 'r':		/* Rotation, accept angle in degrees */
					float theta;
					std::cout<<"enter angle\n";
					std::cin >> theta;
					if (std::cin) 
					{
						std::cout << "calling rotate\n";
						Rotate(M, theta);
					}
					else
					{
						std::cerr << "invalid rotation angle\n";
						std::cin.clear();
					}						
					break;
				case 's':		/* scale, accept scale factors */
					{	
						float scaleX = 0,scaleY = 0;
						std::cout<<"enter scaleX (width)\n";
						std::cin >> scaleX;
						std::cout<<"enter scaleY (heigth)\n";
						std::cin >> scaleY;
						if(!scaleX || !scaleY)
						{
							std::cout << "cannot set scale factor as 0" <<std::endl;
						}
						else
						{
							std::cout << "calling Scale\n";
							Scale(M,scaleX,scaleY);
						}
					}
					break;
				case 't':		/* Translation, accept translations */
					{
						float transX = 0,transY = 0;
						std::cout<<"enter transX\n";
						std::cin >> transX;
						std::cout<<"enter transY\n";
						std::cin >> transY;
						std::cout << "calling Translate\n";
						Translate(M,transX,transY);
					}
					break;
				case 'h':		/* Shear, accept shear factors */
					{
						float shearX = 0,shearY = 0;
						std::cout<<"enter shearX\n";
						std::cin >> shearX;
						std::cout<<"enter shearY\n";
						std::cin >> shearY;
						std::cout << "calling Shear\n";
						Shear(M,shearX,shearY);
					}
					break;
				case 'f':		/* Flip, accept flip factors */
					{
						int flipX = 0;
						int flipY = 0;
						std::cout << "Enter 1 to flip horizontally else 0\n";
						std::cin >>flipX;
						std::cout << "Enter 1 to flip vertically else 0\n";
						std::cin >>flipY;
						if(flipX || flipY)
						{
							Flip(M,flipX,flipY);
						}
					}
					break;
				case 'p':		/* Perspective, accept perspective factors */
					{
						float perspecX = 0,perspecY = 0;
						std::cout<<"enter perspective factor Px\n";
						std::cin >> perspecX;
						std::cout<<"enter perspective factor Py\n";
						std::cin >> perspecY;
						std::cout << "calling Perspective()\n";
						Perspective(M,perspecX,perspecY);
					}
					break;		
				case 'd':		/* Done, that's all for now */
					break;
				default:
					std::cout << "invalid command, enter r, s, t, h, f, p, d\n";
			}
		}
	} while (cmd.compare("d")!=0);

}
