

# Argon
A simple & lightweight wrapper for SDL2 that provides a clean and easy API, allowing you to create shorter graphical projects in c++. 

This wrapper also wraps some SDL_GFX functions and SDL_TTF functions, but in order to stay lightweight you must add the appropriate `#define` if you wish to include these parts of the wrapper. Use `#define ARGON_INCLUDE_GFX` or `#define ARGON_INCLUDE_TTF` respectively before `#define "Argon.h"`.

Please take a look at the examples folder for some use cases.

## Getting Started
Ensure all prerequisites are fulfilled then copy the header file into your source code and add `#include "Argon.h"` to the source file where you wish to use the wrapper.

### Prerequisites
- [SDL2](https://www.libsdl.org/download-2.0.php)

### Basic Usage

Argon is a class that can be extended, you then have full access to the API. View examples for more details.
```C++
#include "Argon.h"

struct MyApp : public Argon {

	// Required sets up  settings for creating the window when begin() is called
	MyApp() : Argon("App Title", 800, 800, ARGON_RESIZABLE|ARGON_HIGHDPI|ARGON_VSYNC) {}

	void gameLoop(double dt) {
		// Runs once per frame (dt is the deltaTime between now and the last frame in seconds)

		// Some convenient getters (see API section for all getters)
		// mx() mouse x position
		// my() mouse y position
		// mouseDown() returns true if mouse is down
		// mWhich() returns which button is pressed (or last pressed)
		// wx() current window x position
		// wy() current window y position
		// ww() current window width
		// wh() current window height
		

		// Check API for auto handling of resizable textures & more
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

### Constructors
The constructor sets up all of the settings for the project. Use `begin()` to actually create the window. 

#### Variations
```C++
Argon(const char* title, int32_t x, int32_t y, int32_t w, int32_t h, uint16_t flags);
Argon(const char* title, int32_t w, int32_t h, uint16_t flags);
Argon(const char* title, int32_t w, int32_t h);
Argon(const char* title, uint16_t flags);
Argon(const char* title);
Argon();
```
#### Arguments
`title`: the title of the window argon creates. Defaults to "Argon".
`x`: the x position of the window. Defaults to SDL_WINDOWPOS_UNDEFINED.
`y`: the y position of the window. Defaults to SDL_WINDOWPOS_UNDEFINED.
`w`: the width of the window. Defaults to 500.
`h`: the height of the window. Defaults to 500.
`flags`: bitfield options created by performing the 'or' operation on the following flags. Defaults to ARGON_VSYNC|ARGON_RESIZABLE|ARGON_HIGHDPI.
- `ARGON_FULLSCREEN`: start in full screen mode
- `ARGON_HIDDEN`: start with window hidden
- `ARGON_BORDERLESS`: do not show border on window (if enabled)
- `ARGON_RESIZABLE`: allow window to be resizable
- `ARGON_MINIMIZED`: start with window minimized
- `ARGON_MAXIMIZED`: start with window maximized
- `ARGON_HIGHDPI`: use high dpi mode for better resolutions
- `ARGON_ON_TOP`: force window to always appear on top of others
- `ARGON_NO_TASKBAR`: hide taskbar
- `ARGON_MOUSE_CAPTURE`: do not show mouse (like in most video games)
- `ARGON_VSYNC`: enable vertical synchronization

### General Functions
`void begin()`: create the window and allow all Argon functionality  
`void stop()`: destroy the window and stop all Argon functionality  
`void maximize()`: maximize window  
`void minimize()`: minimize window  
`void show()`: show window  
`void hide()`: hide window  
`void raise()`: raise window  
`void restore() `: restore window  
`void fullscreen()`: put window into full screen mode  
`void setMaxSize(int w, int h)`: set maximum window size  
`void setMinSize(int w, int h)`: set minimum window size  
`void setOpacity(float opacity)`: set window opacity  
`void setPosition(int x, int y)`: set window position  
`void setMousePosition(int x, int y)`: set mouse position  
`void setSize(int w, int h)`: set window size  
`void setTitle(const char* title)`: set window title  
`void setIsResizable(bool resizable)`: enable or disable window resizability   
`void setBordered(bool bordered)`: enable or disable window borders  
`void setBrightness(float brightness)`: set brightness of windows  
`bool setCaptureMouse(bool capture = true)`: enable or disable capturing of the mouse  
`bool setRelativeMouseMode(bool relative)`: enable relative mouse mode (mouse movements described in offsets, see `rx` and `ry` in `MouseEvent`)  
`void setCursorDisplay(bool show)`: show or hide the cursor  
`bool alert(const char* message)`: trigger a native alert with the message  
`bool alert(const char* title, const char* message)`:  trigger a native alert with the message and title  
`int alert(const char* title, const char* message, uint8_t numButtons, SDL_MessageBoxButtonData* btnData, SDL_MessageBoxFlags flag = SDL_MESSAGEBOX_INFORMATION)`: trigger a native alert with the message and buttons...  



## Authors
  - **Owen Kuhn** - *Development* -
    [Github](https://github.com/OwenK2)


## License
This project is licensed under the [MIT License](LICENSE)

