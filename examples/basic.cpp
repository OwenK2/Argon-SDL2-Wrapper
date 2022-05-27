/**
 * Example created by Owen Kuhn (author of Argon)
 * To compile run `g++ -std=c++17 -o example basic.cpp -lSDL2`
 * Creates a window with a square in the middle, useful as a minimal starting point
*/


#include "../src/Argon.h"

inline int16_t min(int16_t a, int16_t b) {return a < b ? a : b;}


struct MyApp : public Argon {
	MyApp() : Argon("App Title", 800, 800, ARGON_RESIZABLE|ARGON_HIGHDPI|ARGON_VSYNC) {}

	void gameLoop(double dt) {
		int32_t cx = ww() / 2; // Center x pos: window width / 2
		int32_t cy = wh() / 2; // Center y pos: window height / 2
		int16_t size = min(ww() / 2, wh() / 2); // Size of square sides
		fillRect(cx - size/2, cy - size/2, size, size, 0x00FFFFFF);
	}
};

int main(int argc, char** argv) {

	MyApp app;
	app.begin();

	return 0;
}