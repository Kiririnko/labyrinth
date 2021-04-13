#pragma once
#ifndef WINDOW_H_
#define WINDOW_H_

#include <memory>
#include <iostream>
#include <stdexcept>
#include <SDL2/SDL.h>

class Window
{
public:
	static constexpr int DEFAULT_WIDTH = 1280;
	static constexpr int DEFAULT_HEIGHT = 720;

protected:
	std::shared_ptr<SDL_Window> _window;
	std::shared_ptr<SDL_Renderer> _renderer;

	int _width, _height;

public:
	Window(int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT);
	virtual ~Window() = default;
	//Window(const Window &other) = delete;
	//Window(Window &&other) = default;

	int width() const { return _width; }
	int height() const { return _height; }

	void main_loop();

	virtual void setup() { }
	virtual void handle_event(const SDL_Event & event) {}
	virtual void handle_keys(const Uint8 * keys) {}
	virtual void do_logic() { }
	virtual void render() { }
};


#endif /* WINDOW_H_ */
