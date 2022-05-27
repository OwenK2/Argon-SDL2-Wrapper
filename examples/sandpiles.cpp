/**
 * Example created by Owen Kuhn (author of Argon)
 * https://en.wikipedia.org/wiki/Abelian_sandpile_model
 * To compile run `g++ -std=c++17 -Xpreprocessor -O3 -o sandpiles sandpiles.cpp -lSDL2 -lomp`
 * CONTROLS
 * Add 1 grain of sand: click
 * Clear table: 0 key or R key
 * Set whole table to 1 grain: 1 key
 * Set whole table to 2 grains: 2 key
 * Set whole table to 3 grains: 3 key
 * Place a lot of sand at the center: 4 key
 * Increase iterations per frame (faster): +
 * Decrease iterations per frame (slower): - 
*/

#include "../src/Argon.h"
#include <chrono>

struct SandPiles : public Argon {
	static constexpr uint16_t w = 600;
	static constexpr uint16_t h = 600;
	static constexpr uint32_t spawnRate = 100;
	// static constexpr uint32_t colorPalette[5] = {0x311E10FF, 0xECE4B7FF, 0xEABE7CFF, 0xD9DD92FF, 0xDD6031FF};
	static constexpr uint32_t colorPalette[5] = {0x000000FF, 0x14007BFF, 0x9D009BFF, 0xF78500FF, 0xFFFEEFFF};

	Texture* frame;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastAddition;
	uint16_t topplePerFrame = 12;
	uint32_t* sand;
	uint32_t* tmp;

	struct {
		bool minus : 1;
		bool plus : 1;
	} keys;


	SandPiles() : Argon("Sand Piles", w, h, ARGON_HIGHDPI|ARGON_VSYNC) {}
	void onLoad(WindowEvent event) {
		frame = createTexture();

		sand = new uint32_t[w*h];
		tmp = new uint32_t[w*h];
		memset(&keys, 0, sizeof(keys));
		setSand(sand, 0);

		lastAddition = std::chrono::high_resolution_clock::now();
	}
	void gameLoop(double dt) {
		if(mouseDown()) {addSand(mx(), my());}
		if(keys.plus && !keys.minus) {
			topplePerFrame += 2;
		}
		else if(keys.minus && !keys.plus) {
			topplePerFrame -= 2;
		}


		for(uint16_t i = 0;i < topplePerFrame; ++i){topple();}
		
		updateTexture();
		drawTexture(frame);
	}
	void onKeyDown(KeyEvent event) {
		if(event.keycode == SDLK_MINUS) {
			keys.minus = true;
		}
		else if(event.keycode == SDLK_EQUALS) {
			keys.plus = true;
		}
	}
	void onKeyUp(KeyEvent event) {
		if(event.keycode == SDLK_MINUS) {
			keys.minus = false;
		}
		else if(event.keycode == SDLK_EQUALS) {
			keys.plus = false;
		}
		else if(event.keycode == SDLK_r || event.keycode == SDLK_0) {
			setSand(sand, 0);
		}
		else if(event.keycode == SDLK_1) {
			setSand(sand, 1);
		}
		else if(event.keycode == SDLK_2) {
			setSand(sand, 2);
		}
		else if(event.keycode == SDLK_3) {
			setSand(sand, 3);
		}
		else if(event.keycode == SDLK_4) {
			setSand(sand, 0);
			sand[(h/2) * w + (w/2)] = h*w*10;
		}
	}

	void addSand(int32_t x, int32_t y) {
		if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastAddition).count() >= spawnRate) {
			++sand[y * w + x];
			lastAddition = std::chrono::high_resolution_clock::now();
		}
	}

	bool topple() {
		bool changes = false;
		setSand(tmp, 0);

		for(uint32_t y = 0;y < h; ++y) {
			for(uint32_t x = 0;x < w; ++x) {
				size_t i = y * w + x;
				tmp[i] += sand[i];
				if(sand[i] >= 4) {
					tmp[i] -= 4;
					if(x > 0) {++tmp[i-1];}
					if(x < w-1) {++tmp[i+1];}
					if(y > 0) {++tmp[i-w];}
					if(y < h-1) {++tmp[i+w];}
					changes = true;
				}
			}
		}	

		uint32_t* _tmp = sand;
		sand = tmp;
		tmp = _tmp;
		return changes;
	}

	void updateTexture() {
		int32_t w, h;
		uint8_t* pixels = lockTexture(frame, &w, &h);
		if(pixels != NULL) {
			#pragma omp parallel for schedule(dynamic)
			for(size_t i = 0;i < w*h; ++i) {
				uint32_t color = sand[i] >= 4 ? colorPalette[4] : colorPalette[sand[i]];
				pixels[i*4] = (color >> 24) & 0xFF;
				pixels[i*4+1] = (color >> 16) & 0xFF;
				pixels[i*4+2] = (color >> 8) & 0xFF;
				pixels[i*4+3] = color & 0xFF;
			}
		}
		unlockTexture(frame);
	}

	void setSand(uint32_t* sand, uint32_t val) {
		#pragma omp parallel for schedule(dynamic)
		for(size_t i = 0; i < w*h; ++i) {
			sand[i] = val;
		}
	}
};

// Run Code
int main(int argc, char** argv) {
	SandPiles sp;
	sp.begin();
	return 0;
}