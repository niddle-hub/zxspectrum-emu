/*
 * Window.h
 *
 *  Created on: 25 июн. 2020 г.
 *      Author: Даниил
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include <memory>
#include <SDL2/SDL.h>

#include "Memory.h"
#include "IO.h"
#include "Z80CPU.h"

class Window{
public:
	static constexpr int DEFAULT_WIDTH = 1400;
	static constexpr int DEFAULT_HEIGHT = 1000;
	static constexpr int PIXEL_SCALE = 5;
protected:
	int _width, _height;
	std::shared_ptr<SDL_Window> _window;
	std::shared_ptr<SDL_Renderer> _renderer;
	bool _want_quit {false};

	void handle_event(const SDL_Event & event);
	void handle_keys(const Uint8 *keys);
	void do_logic();
	void render();
public:
	Window(int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT);
	Window(const Window &) = delete;
	Window(Window &&) = delete;

	void main();

protected:
	ROM rom{"zx.rom"};
	RAM ram { 16 };
	IO io;
	AddressSpace system_bus{ram, rom, io};
	Z80CPU cpu {system_bus};
};


#endif /* WINDOW_H_ */
