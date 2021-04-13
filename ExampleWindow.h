#ifndef EXAMPLEWINDOW_H_
#define EXAMPLEWINDOW_H_

#include "Window.h"
#include <vector>

class ExampleWindow: public Window
{
protected:
	int fov;
	float d0;
	std::shared_ptr<SDL_Texture> wall;
	std::vector<uint32_t> map_img;
	SDL_Rect rectangle_map, imgPartRect;
	unsigned w_rectagle_map, h_rectagle_map, countColomnInRectangle;

public:
	ExampleWindow(
			int width = DEFAULT_WIDTH,
			int height = DEFAULT_HEIGHT
			);
	virtual ~ExampleWindow() = default;

	virtual void render() override;
	virtual void do_logic() override;
	virtual void handle_keys(const Uint8 *keys) override;
};


#endif /* EXAMPLEWINDOW_H_ */
