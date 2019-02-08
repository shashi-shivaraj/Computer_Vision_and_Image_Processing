#include "triangle.h"

triangle::triangle(SDL_Renderer* ren, SDL_Window* win,const point &p1,const point &p2,const point &p3,const color &c):
renderer(ren),
window(win),
vertex1(p1.x_pos,p1.y_pos),
vertex2(p2.x_pos,p2.y_pos),
vertex3(p3.x_pos,p3.y_pos),
paint(c.r,c.g,c.b,c.a)
{
	/*std::cout<< "triangle vertices:"<<std::endl;
	std::cout<<"( "<<p1.x_pos<<" , "<<p1.y_pos<<" )"<<std::endl;
	std::cout<<"( "<<p2.x_pos<<" , "<<p2.y_pos<<" )"<<std::endl;
	std::cout<<"( "<<p3.x_pos<<" , "<<p3.y_pos<<" )"<<std::endl;*/
}


void triangle::draw() const
{
	 SDL_SetRenderDrawColor( renderer, paint.r, paint.g, paint.b, paint.a);
	 SDL_RenderDrawLine( renderer, vertex1.x_pos, vertex1.y_pos, vertex2.x_pos, vertex2.y_pos);
	 SDL_RenderDrawLine( renderer, vertex3.x_pos, vertex3.y_pos, vertex2.x_pos, vertex2.y_pos);
	 SDL_RenderDrawLine( renderer, vertex1.x_pos, vertex1.y_pos, vertex3.x_pos, vertex3.y_pos);
}

void triangle::setparams(const point &v1,const point &v2,const point &v3,const color &c)
{
	vertex1.x_pos = v1.x_pos;
	vertex1.y_pos = v1.y_pos;
	vertex2.x_pos = v2.x_pos;
	vertex2.y_pos = v2.y_pos;
	vertex3.x_pos = v3.x_pos;
	vertex3.y_pos = v3.y_pos;
	paint.r = c.r;
	paint.g = c.g;
	paint.b = c.b;
	paint.a = c.a;
	
}


std::ostream& operator <<(std::ostream& cout,const triangle& t)
{
	t.draw();
	return cout;
}
