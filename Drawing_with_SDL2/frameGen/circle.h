#ifndef CIRCLE_H
#define CIRCLE_H

#include "header.h"

class circle
{
	public:
	circle(SDL_Renderer*, SDL_Window*, const unsigned int &, const point &,const color &);
	circle(const circle &) = delete;
	circle & operator =(const circle &) = delete;
	
	void draw();
	void setparams(const int &r,const point &p,const color &c);
	
	std::ostream& operator <<(std::ostream&);
		
	private:
		SDL_Renderer* const renderer;
		SDL_Window* const window;
		int radius;
		point center;
		color paint;
};


#endif /*CIRCLE_H*/
