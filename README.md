

# Argon
A simple & lightweight wrapper for SDL2 that provides a clean and easy API, allowing you to create shorter graphical projects in c++. This wrapper also wraps some SDL_GFX functions and SDL_TTF functions, but in order to stay lightweight you must add the appropriate `#define` if you wish to include these parts of the wrapper. Please take a look at the examples folder for some use cases.

## Getting Started
Ensure all prerequisites are fulfilled then copy the header file into your source code and add `#include "Argon.h"` to the source file where you wish to use the wrapper.

### Prerequisites
- [SDL2](https://www.libsdl.org/download-2.0.php)

### Basic Usage

Argon is a class that can be extended, you then have full access to the API. View examples for more details.
```
#include "Argon.h"

struct MyApp : public Argon {

	// Required sets up  settings for creating the window when begin() is called
	MyApp() : Argon("App Title", 800, 800, ARGON_RESIZABLE|ARGON_HIGHDPI|ARGON_VSYNC) {}

	void gameLoop(double dt) {
		// Runs once per frame (dt is the deltaTime between now and the last frame in seconds)

		// Some convenient getters (see api for all)
		// mx() mouse x position
		// my() mouse y position
		// mouseDown() returns true if mouse is down
		// mWhich() returns which button is pressed (or last pressed)
		// wx() current window x position
		// wy() current window y position
		// ww() current window width
		// wh() current window height
		

		// Check API for auto handling of resizable textures
	}

	// Event Listeners for you to overload...
	void onQuit(const Event event) {}
	void onLoad(const WindowEvent event) {}
	void onUnload(const WindowEvent event) {}
	void onMouseDown(const MouseButtonEvent event) {}
	void onMouseUp(const MouseButtonEvent event) {}
	void onMouseMove(const  MouseEvent event) {}
	void onMouseWheel(const WheelEvent event) {}
	void onKeyDown(const KeyEvent event) {}
	void onKeyUp(const KeyEvent event) {}
	void onWindowShow(const WindowEvent event) {}
	void onWindowExpose(const WindowEvent event) {}
	void onWindowMove(const WindowEvent event) {}
	void onWindowResize(const WindowEvent event) {}
	void onWindowMinimize(const WindowEvent event) {}
	void onWindowMaximize(const WindowEvent event) {}
	void onWindowRestore(const WindowEvent event) {}
	void onMouseEnter(const MouseEvent event) {}
	void onMouseLeave(const MouseEvent event) {}
	void onWindowFocus(const WindowEvent event) {}
	void onWindowBlur(const WindowEvent event) {}
	void onWindowClose(const WindowEvent event) {}
	void onWindowTakeFocus(const WindowEvent event) {}
	void onWindowHitTest(const WindowEvent event) {}
	void onFileDrop(const FileDropEvent event) {}
};

int main(int argc, char** argv) {
	MyApp app;
	app.begin();
	return 0;
}
```

## Running the Examples
All of the examples in the `./examples` folder have comments at the top to explain how to compile and run them

## API


## Authors
  - **Owen Kuhn** - *Development* -
    [Github](https://github.com/OwenK2)


## License
This project is licensed under the [MIT License](LICENSE)

