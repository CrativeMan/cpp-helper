#include "Window.hpp"

Window::Window(int width, int height, char *title){
	this.width = width;
	this.height = height;
	this.*title = *title;
}