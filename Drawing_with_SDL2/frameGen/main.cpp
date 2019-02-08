#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include "frameGenerator.h"
#include "circle.h"
#include "triangle.h"

const std::string TITLE = "shashi-shivaraju's geometrical design";
const std::string NAME = "shonnah";

const int WIDTH = 800;
const int HEIGHT = 700;

void drawCircle(SDL_Renderer* renderer,
  SDL_Point center, int radius, SDL_Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  for (int w = 0; w < radius * 2; w++) {
    for (int h = 0; h < radius * 2; h++) {
      int dx = radius - w; // horizontal offset
      int dy = radius - h; // vertical offset
      if ((dx*dx + dy*dy) <= (radius * radius)) {
        SDL_RenderDrawPoint(renderer, center.x + dx, center.y + dy);
      }
    }
  }
}


void writeName(SDL_Renderer* renderer) {
  TTF_Init();
  TTF_Font* font = TTF_OpenFont("fonts/arial.ttf", 24);
  if (font == NULL) {
    throw std::string("error: font not found");
  }
  SDL_Color textColor = {255,255,255,255};
  SDL_Surface* surface = 
    TTF_RenderText_Solid(font, TITLE.c_str(), textColor);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

  int textWidth = surface->w;
  int textHeight = surface->h;
  SDL_FreeSurface(surface);
  SDL_Rect dst = {20, HEIGHT-40, textWidth, textHeight};

  SDL_RenderCopy(renderer, texture, NULL, &dst);
  SDL_DestroyTexture(texture);
  TTF_CloseFont(font);
}


void createArt(SDL_Window* window,SDL_Renderer* renderer,int width,int height)
{
	//create an circle object
    point cen(width/2,height/2);
    color col(0,0,255,255);
    std::vector<point> inter_points;
    
    //draw exterior circle
    int radius = 300;
    circle circleGen(renderer, window, radius,cen,col);
    circleGen.draw();
    std::cout<<"circle info to demo overloading: "<<std::endl;
    circleGen<<std::cout;
    
    //draw interior circle
    int rad_circle = 200;
    circleGen.setparams(rad_circle,cen,col);
	circleGen.draw();
    
    
    //draw the intersecting circles
    //draw overlapping circles
    int rad =(radius-rad_circle)/2;
    for(float theta = 0;theta<1.99*pi;theta = theta+0.03125)
	{
		int dx = (rad_circle+rad) * std::cos(theta) + width/2;
		int dy = (rad_circle+rad) * std::sin(theta) + height/2; 
		
		cen.x_pos = dx;
		cen.y_pos = dy;
		circleGen.setparams(rad,cen,col);
		circleGen.draw();	
	} 
    
    /*center of the window*/
    cen.x_pos = width/2;
	cen.y_pos = height/2;
    
    //divide the circle into 12 equal parts
	for(float theta = 0;theta<2*pi;theta = theta+(2*pi)/24)
	{
		int dx = rad_circle * std::cos(theta); // horizontal offset
		int dy = rad_circle * std::sin(theta); // vertical offset
		inter_points.push_back(point(cen.x_pos+dx,cen.y_pos+dy));
	}
	
	color c1(225,0,0,255); //red
	//create a  triangle object
	triangle triGen(renderer,window,point(inter_points[1].x_pos,inter_points[1].y_pos),
						point(inter_points[11].x_pos,inter_points[11].y_pos),
						point(cen.x_pos,inter_points[18].y_pos),c1);
	std::cout<<triGen;
	
	//reusing the object
	triGen.setparams(point(inter_points[13].x_pos,inter_points[13].y_pos),
						point(inter_points[23].x_pos,inter_points[23].y_pos),
						point(cen.x_pos,inter_points[6].y_pos),c1);
	std::cout<<triGen;

	c1.r = 0;
	c1.b = 255;
	//reusing the object
	triGen.setparams(point(inter_points[2].x_pos,inter_points[2].y_pos),
						point(inter_points[10].x_pos,inter_points[10].y_pos),
						point(cen.x_pos,inter_points[15].y_pos),c1);
	std::cout<<triGen;
	
	//reusing the object
	triGen.setparams(point(inter_points[14].x_pos,inter_points[14].y_pos),
						point(inter_points[22].x_pos,inter_points[22].y_pos),
						point(cen.x_pos,inter_points[9].y_pos),c1);
	std::cout<<triGen;
	
	c1.b = 0;
	c1.g = 255;
	//reusing the object
	triGen.setparams(point(inter_points[3].x_pos,inter_points[3].y_pos),
						point(inter_points[9].x_pos,inter_points[9].y_pos),
						point(cen.x_pos,inter_points[14].y_pos),c1);
	std::cout<<triGen;
	
	//reusing the object
	triGen.setparams(point(inter_points[15].x_pos,inter_points[15].y_pos),
						point(inter_points[21].x_pos,inter_points[21].y_pos),
						point(cen.x_pos,inter_points[10].y_pos),c1);
	std::cout<<triGen;
}


int main(void) {
  try {
    if ( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
      std::cout << "Failed to initialize SDL2" << std::endl;
      return EXIT_FAILURE;
    }
    SDL_Window* window = SDL_CreateWindow(
      TITLE.c_str(),
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      WIDTH,
      HEIGHT,
      SDL_WINDOW_SHOWN
    );
    
    // Apparently we can't use hardware acceleration with
    // SDL_GetWindowSurface
    SDL_Renderer* renderer = SDL_CreateRenderer( 
      window, -1, SDL_RENDERER_SOFTWARE
    );

    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
    SDL_RenderClear(renderer);

	SDL_Rect r;
	r.x = 0;
	r.y = 0;
	r.w = WIDTH;
	r.h = HEIGHT;

	// First set the blend mode so that alpha blending will work;
	// the default blend mode is SDL_BLENDMODE_NONE!
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255);

	// Render rect
	SDL_RenderFillRect( renderer, &r );
	
	// draw the design
    createArt(window,renderer,WIDTH,HEIGHT);

    writeName(renderer);
    SDL_RenderPresent(renderer);
    FrameGenerator frameGen(renderer, window, WIDTH, HEIGHT, NAME);
    frameGen.makeFrame();

    SDL_Event event;
    const Uint8* keystate;
    while ( true ) {
      keystate = SDL_GetKeyboardState(0);
      if (keystate[SDL_SCANCODE_ESCAPE]) { break; }
      if (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          break;
        }
      }
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit(); 
  SDL_Quit();
  }
  catch (const std::string& msg) { std::cout << msg << std::endl; }
  catch (...) {
    std::cout << "Oops, someone threw an exception!" << std::endl;
  }
  return EXIT_SUCCESS;
}
