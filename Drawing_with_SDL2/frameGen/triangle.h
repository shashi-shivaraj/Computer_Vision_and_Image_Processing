#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "header.h"

class triangle
{
	public:
	triangle(SDL_Renderer*, SDL_Window*,const point &,const point &,const point &,const color &);
	triangle(const triangle &) = delete;
	triangle & operator =(const triangle &) = delete;
	
	void draw()const;
	void setparams(const point &v1,const point &v2,const point &v3,const color &c);
		
	private:
		SDL_Renderer* const renderer;
		SDL_Window* const window;
		point vertex1;
		point vertex2;
		point vertex3;
		color paint;
};

std::ostream& operator <<(std::ostream&,const triangle&);


#endif /*TRIANGLE_H*/
