#include "ExampleWindow.h"
#include <SDL2/SDL_image.h>
#include <stdexcept>

#include <cmath>

constexpr float eps = 0.001;
constexpr int MAP_W = 15, MAP_H = 15;
const char map[] =
  "###############"
  "#             #"
  "####          #"
  "#  #          #"
  "#  ####   #####"
  "#     #   #   #"
  "#     #   #   #"
  "#     ## ##   #"
  "#         #   #"
  "####   ####   #"
  "#      #      #"
  "#      #      #"
  "#             #"
  "#      #      #"
  "###############";

float dx, dy, rx, ry, Dh, Dv, D, tx, ty, cosB, sinB, alfa = 45,
alfaRad, gammaRad, betaRad, d, x = 2, y = 10, stepX, stepY, step = 0.1;
int h, colomn, colomnExampleWindow, wExampleWindow, hExampleWindow;

void ray_cast(float &rx, float &ry, float dx, float dy){
	do {
		rx += dx;
		ry += dy;
	} while (map[(int)rx+(int)ry*MAP_W] !='#' && ((0<=ry) && (y<=MAP_H) && (0<=rx) && (rx<=MAP_W)));
}

ExampleWindow::ExampleWindow(int width, int height):
	Window(width, height),
	fov(90), d0(width/(2*tan(fov/2 * 3.14 / 180))),
	map_img(width*height/4), rectangle_map(),
	w_rectagle_map((width/6)/MAP_W), h_rectagle_map((height/6)/MAP_H), countColomnInRectangle(width / MAP_W){
	std::shared_ptr<SDL_Surface> surf ;
	surf = std::shared_ptr<SDL_Surface>(
				IMG_Load("wall.png"),SDL_FreeSurface);
	if (surf == nullptr)
			throw std::runtime_error(
				std::string("Не удалось загрузить поверхность: ")
					+ std::string(SDL_GetError()));
	wall = std::shared_ptr<SDL_Texture>(
		SDL_CreateTextureFromSurface(_renderer.get(),surf.get()), SDL_DestroyTexture);
	if (wall == nullptr)
		throw std::runtime_error(
			std::string("Не удалось загрузить текстуру: ")
					+ std::string(SDL_GetError()));
	SDL_QueryTexture(wall.get(), nullptr, nullptr, &wExampleWindow, &hExampleWindow);
}

void ExampleWindow::render() {
	SDL_SetRenderDrawColor(_renderer.get(), 100, 100, 100, 255);
	SDL_RenderClear(_renderer.get());
	SDL_SetRenderDrawBlendMode(_renderer.get(), SDL_BLENDMODE_BLEND);
	rectangle_map.w = width();
	rectangle_map.h = height()/2;
	rectangle_map.x = 0;
	rectangle_map.y = 0;
	SDL_SetRenderDrawColor(_renderer.get(), 200, 200, 200, 255);
	SDL_RenderFillRect(_renderer.get(), &rectangle_map);
	SDL_SetRenderDrawColor(_renderer.get(), 255, 0, 0, 255);
	for (colomn = 0;  colomn < width(); ++colomn) {
		gammaRad = atan2(colomn-width()/2, d0);
		alfaRad = alfa * 3.14 / 180;
		betaRad = gammaRad + alfaRad;
		cosB = cos(betaRad);
		sinB = sin(betaRad);
		if (cosB > eps){
			rx = floor(x) + eps;
			dx = 1;
			dy = tan(betaRad);
			ry = y- (x-rx) * dy;
		}
		else if (cosB < -eps){
			rx = ceil(x) - eps;
			dy = tan(-betaRad);
			dx = -1;
			ry = y - (rx - x) * dy;
		}
		ray_cast(rx, ry, dx, dy);
		ty = ry - floor(ry);
		if (((-eps <= cosB) && (cosB <= eps)) || (rx<0 || rx > MAP_W || ry <0 || ry >MAP_H)){
			Dv = width()*3;
		}
		else {
			Dv = hypot (rx-x, ry-y);
		}
		if (sinB > eps){
			dy = 1;
			ry = floor(y) + eps;
			dx = 1/tan(betaRad);
			rx = x - (y - ry) * dx;
		}
		else if (sinB < -eps){
			dy = -1;
			ry = ceil(y) - eps;
			dx = 1/tan(-betaRad);
			rx = x - (ry - y) * dx;
		}
		ray_cast(rx, ry, dx, dy);
		if (((-eps <= sinB) && (sinB <= eps)) || (rx<0 || rx > MAP_W || ry <0 || ry >MAP_H)){
			Dh = height()*3;
		}
		else {
			Dh = hypot (rx-x, ry-y);
		}
		D = std::min(Dh, Dv);
		d = d0/cos(gammaRad);
		h = d/D;
		if (D == Dh) tx = rx -floor(rx);
		else tx = ty;
	    colomnExampleWindow = colomn % countColomnInRectangle;
		imgPartRect.x = floor (wExampleWindow*tx);
		imgPartRect.y = 0;
		imgPartRect.w = 1;
		imgPartRect.h = hExampleWindow;
		SDL_Rect wall_rect{colomn, height()/2 - h/2, 1, h};
		SDL_RenderCopy(_renderer.get(), wall.get(), &imgPartRect, &wall_rect);
	}
	for (unsigned j=0; j<MAP_H; j++) {
		for (unsigned i=0; i<MAP_W; i++) {
			rectangle_map.w = std::max(w_rectagle_map, h_rectagle_map);
			rectangle_map.h = std::max(w_rectagle_map, h_rectagle_map);
			rectangle_map.x = rectangle_map.w * i;
			rectangle_map.y = rectangle_map.h * j;
			SDL_SetRenderDrawColor(_renderer.get(), 255, 255, 255, 60);
			if (i == (unsigned)x && j == (unsigned)y && map[i+j*MAP_W]==' ') {
				SDL_SetRenderDrawColor(_renderer.get(), 255, 0, 0, 200);
				rectangle_map.w -=rectangle_map.w/2;
				rectangle_map.h -=rectangle_map.h/2;
				rectangle_map.x += rectangle_map.w/2;
				rectangle_map.y += rectangle_map.h/2;
			}
			else if (map[i+j*MAP_W]==' '){
				continue;
			}
			SDL_RenderFillRect(_renderer.get(), &rectangle_map);
		}
	}
}

void ExampleWindow::do_logic() {

}

void ExampleWindow::handle_keys(const Uint8 *keys) {
	if (keys[SDL_SCANCODE_UP]){
		stepX = cos(alfa * 3.14/180)*step;
		stepY = sin(alfa * 3.14/180)*step;
		if(map[(int)(floor(x+stepX))+(int)(floor(y+stepY))*MAP_W]==' '){
			x += stepX;
			y += stepY;
		}
	}
	if (keys[SDL_SCANCODE_DOWN]){
		stepX = cos(alfa * 3.14/180)*step;
		stepY = sin(alfa * 3.14/180)*step;
		if(map[(int)(floor(x-stepX))+(int)(floor(y-stepY))*MAP_W]==' '){
			x -= stepX;
			y -= stepY;
		}
	}
	if (keys[SDL_SCANCODE_LEFT]){
		alfa -=1;
	}
	if (keys[SDL_SCANCODE_RIGHT]){
		alfa +=1;
	}

}
