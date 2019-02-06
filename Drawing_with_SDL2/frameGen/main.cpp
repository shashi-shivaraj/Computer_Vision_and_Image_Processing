#include <iostream>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include "frameGenerator.h"
#include "circle.h"

const std::string TITLE = "shashi-shivaraju's design";
const std::string NAME = "shashi-shivaraju";

const int WIDTH = 720;
const int HEIGHT = 480;

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
  SDL_Color textColor = {0, 0, 0, 0};
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
    color blk(0,0,0,255);
    
    int rad_circle1 = 200;
    int rad_circle2 = 140;
    
    //draw exterior circle
    circle circleGen(renderer, window, rad_circle1,cen,blk);
    circleGen.draw();
    std::cout<<"circle info: "<<std::endl;
    circleGen<<std::cout;
    
    //draw interior circle
	circleGen.setparams(rad_circle2,cen,blk);
	circleGen.draw();	
	std::cout<<"circle info: "<<std::endl;
    circleGen<<std::cout;
    
    int rad =(rad_circle1-rad_circle2)/2;
    //draw overlapping circles
    for(float theta = 0;theta<1.99*pi;theta = theta+0.03125)
	{
		int dx = (rad_circle2+rad) * std::cos(theta) + width/2;
		int dy = (rad_circle2+rad) * std::sin(theta) + height/2; 
		
		cen.x_pos = dx;
		cen.y_pos = dy;
		circleGen.setparams(rad,cen,blk);
		circleGen.draw();	
	} 
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

   // SDL_Point center = {640, 360};
    //SDL_Color black = {0,0,0,255};
  //  drawCircle(renderer, center, 50, black);
  
  
    createArt(window,renderer,WIDTH,HEIGHT);

    writeName(renderer);
    SDL_RenderPresent(renderer);
    FrameGenerator frameGen(renderer, window, WIDTH, HEIGHT, NAME);
    //FrameGenerator frameGen2 = frameGen;
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
