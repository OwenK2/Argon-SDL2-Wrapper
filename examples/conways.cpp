/**
 * Example created by Owen Kuhn (author of Argon)
 * https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
 * To compile run `g++ -std=c++17 -o conways conways.cpp -lSDL2`
 * CONTROLS
 * Use Space to pause or unpause (must be paused to add cells)
 * Mouse down to add cells when paused
 * Use numbers 0-9 to change brushes when paused
 * Enter or R key to reset board
 * Use +/- to speed up or slow down animation (iterations per second)
*/



#include "../src/Argon.h"
#include <chrono>
#include <bitset>
#include <vector>
#include <initializer_list>

#define MAP(x, a, b, c, d) ((float)(c) + (((float)(d) - (float)(c)) / ((float)(b) - (float)(a))) * ((float)(x) - (float)(a)))
float hue2rgb(float p, float q, float h);
uint32_t hsla2rgba(float h, float s, float l, float a = 1.0);

struct Brush {
	const uint8_t w;
	const uint8_t h;
	const std::vector<bool> data;
};


struct Conways : public Argon {
	static constexpr uint16_t SIZE = 51;
	static constexpr uint8_t MIN_UPDATES_PER_SEC = 1;
	static constexpr uint8_t MAX_UPDATES_PER_SEC = 50;

	static const Brush pointBrush;
	static const Brush blockBrush;
	static const Brush beeHiveBrush;
	static const Brush blinkerBrush;
	static const Brush beaconBrush;
	static const Brush pulsarBrush;
	static const Brush pentadecathlonBrush;
	static const Brush gliderBrush;
	static const Brush lwssBrush;
	static const Brush mwssBrush;


	Texture* frame = NULL;
	const Brush* brush = &pointBrush;
	bool playing = false;
	std::bitset<SIZE*SIZE> data;
	std::bitset<SIZE*SIZE> tmp;
	uint32_t updatesPerSec = 8;
	uint32_t msSinceUpdate = 0;

	Conways() : Argon("Conway's Game of Life", 800, 800, ARGON_RESIZABLE|ARGON_HIGHDPI|ARGON_VSYNC) {}
	void onLoad(WindowEvent event) {
		frame = createTexture();
		data.reset();
		setMinSize(SIZE, SIZE);
	}
	void gameLoop(double dt) {
		msSinceUpdate += (uint32_t)(dt*1000);
		if(!playing) {
			if(mouseDown()) {
				const uint8_t ratio = ww() < wh() ? ww() / SIZE : wh() / SIZE;
				const uint32_t boardSize = ratio * SIZE;
				const int32_t xPad = (ww() - boardSize) / 2;
				const int32_t yPad = (wh() - boardSize) / 2;
				const int32_t dx = (mx() - xPad) / ratio;
				const int32_t dy = (my() - yPad) / ratio;
				for(uint8_t y = 0; y < brush->h; ++y) {
					if(dy + y >= SIZE) {break;}
					for(uint8_t x = 0; x < brush->w; ++x) {
						if(dx + x < SIZE && brush->data[y * brush->w + x]) {
							data[(dy + y) * SIZE + (dx + x)] = mWhich() == 1;
						}
					}
				}
			}
			msSinceUpdate = 0;
		}
		else if(msSinceUpdate >= 1000 / updatesPerSec) {
			applyRules();
			msSinceUpdate = 0;
		}
		render();
	}
	void onKeyUp(KeyEvent event) {
		switch(event.keycode) {
			case SDLK_SPACE: playing = !playing; break;
			case SDLK_PLUS:
			case SDLK_EQUALS:
				if(updatesPerSec < MAX_UPDATES_PER_SEC) {++updatesPerSec;}
				break;
			case SDLK_MINUS:
			case SDLK_UNDERSCORE:
				if(updatesPerSec > MIN_UPDATES_PER_SEC) {--updatesPerSec;}
				break;
			case SDLK_r:
			case SDLK_RETURN:
				data.reset();
				break;
			case SDLK_1:
				brush = &pointBrush;
				break;
			case SDLK_2:
				brush = &blockBrush;
				break;
			case SDLK_3:
				brush = &beeHiveBrush;
				break;
			case SDLK_4:
				brush = &blinkerBrush;
				break;
			case SDLK_5:
				brush = &beaconBrush;
				break;
			case SDLK_6:
				brush = &pulsarBrush;
				break;
			case SDLK_7:
				brush = &pentadecathlonBrush;
				break;
			case SDLK_8:
				brush = &gliderBrush;
				break;
			case SDLK_9:
				brush = &lwssBrush;
				break;
			case SDLK_0:
				brush = &mwssBrush;
				break;
		}
	}


	void applyRules() {
		tmp = data;
		for(uint16_t i = 0;i < SIZE*SIZE; ++i) {
			uint8_t numNeighbors = 0;
			uint16_t x = i % SIZE, y = i / SIZE;
			if(x > 0 && y > 0) {numNeighbors += tmp[i-1-SIZE];}
			if(y > 0) {numNeighbors += tmp[i-SIZE];}
			if(x < SIZE-1 && y > 0) {numNeighbors += tmp[i+1-SIZE];}
			if(x > 0) {numNeighbors += tmp[i-1];}
			if(x < SIZE-1) {numNeighbors += tmp[i+1];}
			if(x > 0 && y < SIZE-1) {numNeighbors += tmp[i-1+SIZE];}
			if(x < SIZE-1) {numNeighbors += tmp[i+SIZE];}
			if(x < SIZE-1 && y < SIZE-1) {numNeighbors += tmp[i+1+SIZE];}
			if((tmp[i] && numNeighbors < 2) || numNeighbors > 3) {
				data[i] = 0;
			}
			else if(!tmp[i] && numNeighbors == 3) {
				data[i] = 1;
			}
			else {data[i] = tmp[i];}
		}
	}

	void render() {
		int32_t w, h;
		uint8_t* pixels = lockTexture(frame, &w, &h);
		if(pixels != NULL) {
			const uint8_t ratio = w < h ? w / SIZE : h / SIZE;
			const uint32_t boardSize = ratio * SIZE;
			const uint32_t xPad = (w - boardSize) / 2;
			const uint32_t yPad = (h - boardSize) / 2;
			const float maxd = boardSize * boardSize * 2;
			const uint32_t dmx = (mx()-xPad) / ratio;
			const uint32_t dmy = (my()-yPad) / ratio;

			uint32_t dx, dy;
			for(uint32_t y = 0; y < boardSize; ++y) {
				dy = y / ratio;
				for(uint32_t x = 0; x < boardSize; ++x) {
					dx = x / ratio;
					uint8_t* px = &pixels[((y+yPad) * w + (x+xPad)) * 4];
					if(!playing && ratio > 3 && (y % (uint32_t)ratio == 0 || x % (uint32_t)ratio == 0)) {
						px[0] = 64;
						px[1] = 64;
						px[2] = 64;
						px[3] = 255;
					}
					else if(data[dy * SIZE + dx]) {
						uint32_t color = playing ? hsla2rgba(MAP(x*x+y*y, 0, maxd, 0, 360), 1.0, 0.50) : 0xFFFFFFFF;
						px[0] = (color >> 24) & 0xFF;
						px[1] = (color >> 16) & 0xFF;
						px[2] = (color >> 7) & 0xFF;
						px[3] = color & 0xFF;
					}
					else if(!playing && (dx >= dmx && dy >= dmy && dx < dmx+brush->w && dy < dmy+brush->h && brush->data[(dy - dmy) * brush->w + (dx - dmx)])) {
						px[0] = 128;
						px[1] = 128;
						px[2] = 128;
						px[3] = 255;
					}
					else {
						px[0] = 0;
						px[1] = 0;
						px[2] = 0;
						px[3] = 255;
					}
					
				}
			}
			//Borders
			for(uint32_t x = 0; x < boardSize; ++x) {
				memset(&pixels[((yPad) * w + (x+xPad))*4], 255, 4);
				memset(&pixels[((h-yPad-1) * w + (x+xPad))*4], 255, 4);
			}
			for(uint32_t y = 0; y < boardSize; ++y) {
				memset(&pixels[((y+yPad) * w + (xPad))*4], 255, 4);
				memset(&pixels[((y+yPad) * w + (w-xPad-1))*4], 255, 4);
			}
		}
		unlockTexture(frame);
		drawTexture(frame);
	}	
};

const Brush Conways::pointBrush = {1, 1, {1}};
const Brush Conways::blockBrush = {2, 2, {1, 1, 1, 1}};
const Brush Conways::beeHiveBrush = {4, 3, {0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0}};
const Brush Conways::blinkerBrush = {3, 1, {1, 1, 1}};
const Brush Conways::beaconBrush = {4, 4, {1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1}};
const Brush Conways::pulsarBrush = {13, 13, {0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0}};
const Brush Conways::pentadecathlonBrush = {3, 8, {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1}};
const Brush Conways::gliderBrush = {3, 3, {0, 0, 1, 1, 0, 1, 0, 1, 1}};
const Brush Conways::lwssBrush = {5, 4, {1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1}};
const Brush Conways::mwssBrush = {6, 5, {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1}};

// Run Code
int main(int argc, char** argv) {
	Conways conway;
	conway.begin();
	return 0;
}

float hue2rgb(float p, float q, float h) {
	if(h < 0.0) {++h;}
	if(h > 1.0) {--h;}
	if(h < 1.0/6.0) {return p + (q-p) * 6.0 * h;}
	else if(h < 1.0/2.0) {return q;}
	else if(h < 2.0/3.0) {return p + (q - p) * (2.0/3.0 - h) * 6.0;}
	return p;
}
uint32_t hsla2rgba(float h, float s, float l, float a) {
	float r, g, b;
	if(s == 0) {r = g = b = l;}
	else {
		h /= 360.0;
		float q = l < 0.5 ? l * (1.0 + s) : (l + s - l * s);
		float p = 2.0 * l - q;
		r = hue2rgb(p, q, h + (1.0/3.0));
		g = hue2rgb(p, q, h);
		b = hue2rgb(p, q, h - (1.0/3.0));
	}
	return ((uint8_t)(r * 255.0) << 24) | ((uint8_t)(g * 255.0) << 16) | ((uint8_t)(b * 255.0) << 8) | (uint8_t)(a * 255.0);
}