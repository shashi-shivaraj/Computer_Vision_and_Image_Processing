#ifndef HEADER_H
#define HEADER_H
#include <iostream>
#include <SDL.h>

const double pi = 3.14159265358;

class point
{
public:
	point(const unsigned int &x,const unsigned int &y):
	x_pos(x),
	y_pos(y)
	{}	
	int x_pos;
	int y_pos;
		
};

class color
{
public:
	color(const unsigned char &red,const unsigned char &green,const unsigned char &blue,const unsigned char &alpha):
	r(red),
	g(green),
	b(blue),
	a(alpha)
	{}
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

#endif  /*HEADER_H*/
