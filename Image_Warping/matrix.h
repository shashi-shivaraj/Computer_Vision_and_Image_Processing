/**************************************************************************
*  FILE NAME	: matrix.cpp
*
*  DESCRIPTION  : Definitions for Matrix manipulation routines.
* 
*  PLATFORM		: Linux
*
*  DATE	               	NAME	        	  	REASON
*  27th Oct,2018        Shashi Shivaraju        CPSC_6040_HW_05
*                       [C88650674]
* Reference :Ioannis Karamouzas, 10/15/18 & D. House
***************************************************************************/
#ifndef MATRIX_H
#define MATRIX_H


/*Header file inclusion*/
#include <cstdio>
#include <cmath>
#include <iostream>


/*macro definations*/
#ifndef PI
#define PI		3.1415926536
#endif

/*structure declarations*/
struct Vector3D{
  double x, y, z;
  Vector3D(): x(0),y(0),z(1) {}
  Vector3D(double x_, double y_, double z_): x(x_), y(y_), z(z_) {}
};

struct Vector2D{
  double x, y;
  Vector2D(): x(0),y(0) {}
  Vector2D(double x_, double y_): x(x_), y(y_) {}
};

class Matrix3D{
private:
  double M[3][3];
  
public:
  Matrix3D();
  Matrix3D(const double coefs[3][3]);
  Matrix3D(const Matrix3D &mat);

  void print() const;

  void setidentity();
  void set(const double coefs[3][3]);
  
  double determinant() const;
  Matrix3D adjoint() const;
  Matrix3D inverse() const;
  
  Vector3D operator*(const Vector2D &v) const;
  Vector3D operator*(const Vector3D &v) const;
  Matrix3D operator*(const Matrix3D &m2) const;
  double *operator[](int i);
};

struct BilinearCoeffs{
  double width, height;
  double a0, a1, a2, a3;
  double b0, b1, b2, b3;
  double c2;
};

/*Fucntion prototypes*/
void setbilinear(double width, double height,
		 Vector2D xycorners[4], BilinearCoeffs &coeff);
void invbilinear(const BilinearCoeffs &c, Vector2D xy, Vector2D &uv);
void read_input(Matrix3D &M,int  &warpMode);
void Rotate(Matrix3D &M, float theta); 
void Flip(Matrix3D &M, int flipX,int flipY);
void Scale(Matrix3D &M, float scaleX,float scaleY);
void Shear(Matrix3D &M, float shearX,float shearY);
void Translate(Matrix3D &M,float transX,float transY);
void Perspective(Matrix3D &M,float perspecX,float perspecY);

#endif /* MATRIX_H*/
