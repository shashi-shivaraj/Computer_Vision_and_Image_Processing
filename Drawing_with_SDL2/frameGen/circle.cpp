#include "circle.h"


circle::circle(SDL_Renderer* ren, SDL_Window* win, const unsigned int &r, const point &p,const color &c):
renderer(ren),
window(win),
radius(r),
center(p.x_pos,p.y_pos),
paint(c.r,c.g,c.b,c.b)
{
	
}

void circle::draw()
{
	int prev_x = 0,prev_y = 0;
	//SDL_SetRenderDrawColor(renderer, 255,0,0,255);
	//SDL_RenderDrawPoint(renderer,center.x_pos,center.y_pos);
	SDL_SetRenderDrawColor(renderer, paint.r, paint.g, paint.b, paint.a);
	for(float theta = 0;theta<=2*pi;theta = theta+0.01)
	{
		
		int dx = radius * std::cos(theta); // horizontal offset
		int dy = radius * std::sin(theta); // vertical offset
		if(!theta)
		{
			prev_x = center.x_pos + dx;
			prev_y = center.y_pos + dy;			
		}
		else
		{
			int curr_x = center.x_pos + dx;
			int curr_y = center.y_pos + dy;
			
			if(!(curr_x == prev_x && curr_y == prev_y))
				SDL_RenderDrawLine(renderer,prev_x,prev_y,curr_x,curr_y);
				
			prev_x = curr_x;
			prev_y = curr_y;
		}
	}
}

void circle::setparams(const int &r,const point &p,const color &c)
{
	radius = r;
	center.x_pos = p.x_pos;
	center.y_pos = p.y_pos;
	paint.r = c.r;
	paint.g = c.g;
	paint.b = c.b;
	paint.a = c.a;
}

std::ostream& circle::operator <<(std::ostream& out)
{
	out << "Circle radius = "<<radius<< " and center is ( "<< center.x_pos<<" , "<<center.y_pos << " )"<<std::endl;
	return out;
}
