/**
 * Example created by Owen Kuhn (author of Argon)
 * https://en.wikipedia.org/wiki/Mandelbrot_set
 * https://en.wikipedia.org/wiki/Julia_set
 * To compile run `g++ -std=c++17 -Xpreprocessor -O3 -o fractal fractal.cpp -lSDL2 -lomp`
 * CONTROLS
 * Translate Fractal: arrow keys, WASD, click + drag
 * Zoom Fractal: +/- keys, scroll
 * Rotate Fractal (only  for  some fractal types): Q (CCW) or E (CW)
 * Toggle Fractal Type: Enter
 * Toggle Coloring Method: Tab
 * Toggle Color Scheme: Shift
 * Right Click: set orbit trap point at location
 * Reset Fractal: zero '0' key
*/

#include "../src/Argon.h"
#include <cmath>
#include <chrono>

typedef long double F;
#define MAP(x, a, b, c, d) ((F)(c) + (((F)(d) - (F)(c)) / ((F)(b) - (F)(a))) * ((F)(x) - (F)(a)))

// Color Utility Stuff
F hue2rgb(F p, F q, F h);
uint32_t hsla2rgba(F h, F s, F l, F a = 1.0);
#define THERMAL_LENGTH 119
uint8_t thermal[THERMAL_LENGTH][3] = {{0 , 0 , 0},{0 , 0 , 36},{0 , 0 , 51},{0 , 0 , 66},{0 , 0 , 81},{2 , 0 , 90},{4 , 0 , 99},{7 , 0 , 106},{11 , 0 , 115},{14 , 0 , 119},{20 , 0 , 123},{27 , 0 , 128},{33 , 0 , 133},{41 , 0 , 137},{48 , 0 , 140},{55 , 0 , 143},{61 , 0 , 146},{66 , 0 , 149},{72 , 0 , 150},{78 , 0 , 151},{84 , 0 , 152},{91 , 0 , 153},{97 , 0 , 155},{104 , 0 , 155},{110 , 0 , 156},{115 , 0 , 157},{122 , 0 , 157},{128 , 0 , 157},{134 , 0 , 157},{139 , 0 , 157},{146 , 0 , 156},{152 , 0 , 155},{157 , 0 , 155},{162 , 0 , 155},{167 , 0 , 154},{171 , 0 , 153},{175 , 1 , 152},{178 , 1 , 151},{182 , 2 , 149},{185 , 4 , 149},{188 , 5 , 147},{191 , 6 , 146},{193 , 8 , 144},{195 , 11 , 142},{198 , 13 , 139},{201 , 17 , 135},{203 , 20 , 132},{206 , 23 , 127},{208 , 26 , 121},{210 , 29 , 116},{212 , 33 , 111},{214 , 37 , 103},{217 , 41 , 97},{219 , 46 , 89},{221 , 49 , 78},{223 , 53 , 66},{224 , 56 , 54},{226 , 60 , 42},{228 , 64 , 30},{229 , 68 , 25},{231 , 72 , 20},{232 , 76 , 16},{234 , 78 , 12},{235 , 82 , 10},{236 , 86 , 8},{237 , 90 , 7},{238 , 93 , 5},{239 , 96 , 4},{240 , 100 , 3},{241 , 103 , 3},{241 , 106 , 2},{242 , 109 , 1},{243 , 113 , 1},{244 , 116 , 0},{244 , 120 , 0},{245 , 125 , 0},{246 , 129 , 0},{247 , 133 , 0},{248 , 136 , 0},{248 , 139 , 0},{249 , 142 , 0},{249 , 145 , 0},{250 , 149 , 0},{251 , 154 , 0},{252 , 159 , 0},{253 , 163 , 0},{253 , 168 , 0},{253 , 172 , 0},{254 , 176 , 0},{254 , 179 , 0},{254 , 184 , 0},{254 , 187 , 0},{254 , 191 , 0},{254 , 195 , 0},{254 , 199 , 0},{254 , 202 , 1},{254 , 205 , 2},{254 , 208 , 5},{254 , 212 , 9},{254 , 216 , 12},{255 , 219 , 15},{255 , 221 , 23},{255 , 224 , 32},{255 , 227 , 39},{255 , 229 , 50},{255 , 232 , 63},{255 , 235 , 75},{255 , 238 , 88},{255 , 239 , 102},{255 , 241 , 116},{255 , 242 , 134},{255 , 244 , 149},{255 , 245 , 164},{255 , 247 , 179},{255 , 248 , 192},{255 , 249 , 203},{255 , 251 , 216},{255 , 253 , 228},{255 , 254 , 239 }};
enum COLOR_PALETTE : uint8_t {
	THERMAL, RGB, BW
};
enum COLOR_METHOD : uint8_t {
	ITERATIONS, POINT_ORBIT_TRAP
};
enum FRACTAL_TYPE : uint8_t {
	MANDELBROT, JULIA
};


// Fractal Class
struct FractalVisualizer : public Argon {
	//Settings
	static constexpr F arrowStep = 5.0;
	static constexpr F zoomFactor = 1.1;
	static constexpr F rotateFactor = 0.05;

	// Variables
	Texture* frame;
	F scale = 1;
	F tx = 0;
	F ty = 0;
	struct {
		bool left  : 1;
		bool up    : 1;
		bool right : 1;
		bool down  : 1;
		bool minus : 1;
		bool plus  : 1;
		bool q     : 1;
		bool e     : 1;
	} keys;

	//Enums
	FRACTAL_TYPE fractalType = MANDELBROT;
	COLOR_METHOD colorMethod = ITERATIONS;
	COLOR_PALETTE colorPalette = THERMAL;

	// Fractal Parameters
	uint32_t maxIterations = 100;
	F angle = 0;
	F maxDistance = 2.47;
	F params[4] = {-2.00, 0.47, -1.00, 1.00};
	F orbitPoint[2] = {0, 0};
	F size[2] = {2.47, 2};

	FractalVisualizer() : Argon("Fractal Visualizer", ARGON_RESIZABLE|ARGON_HIGHDPI|ARGON_VSYNC) {}
	void onLoad(WindowEvent event) {
		frame = createTexture();
		renderFractal();
		memset(&keys, 0, sizeof(keys));
	}
	void onUnload(WindowEvent event) {

	}
	void gameLoop(double dt) {
		bool changes = false;
		if(keys.left)  {tx    -= arrowStep  * scale * dt;changes = true;}
		if(keys.up)    {ty    -= arrowStep  * scale * dt;changes = true;}
		if(keys.right) {tx    += arrowStep  * scale * dt;changes = true;}
		if(keys.down)  {ty    += arrowStep  * scale * dt;changes = true;}
		if(keys.minus) {scale += zoomFactor * scale * dt;changes = true;}
		if(keys.plus)  {scale -= zoomFactor * scale * dt;changes = true;}
		if(keys.q)     {angle -= rotateFactor; changes = true;}
		if(keys.e)     {angle += rotateFactor; changes = true;}
		if(changes)    {renderFractal();}
		drawTexture(frame);
	}
	void onKeyDown(KeyEvent event) {
		if(event.keycode == SDLK_LEFT || event.keycode == SDLK_a) {keys.left = true;}
		else if(event.keycode == SDLK_UP || event.keycode == SDLK_w) {keys.up = true;}
		else if(event.keycode == SDLK_DOWN || event.keycode == SDLK_s) {keys.down = true;}
		else if(event.keycode == SDLK_RIGHT || event.keycode == SDLK_d) {keys.right = true;}
		else if(event.keycode == SDLK_MINUS) {keys.minus = true;}
		else if(event.keycode == SDLK_EQUALS) {keys.plus = true;}
		else if(event.keycode == SDLK_q) {keys.q = true;}
		else if(event.keycode == SDLK_e) {keys.e = true;}
		else if(event.keycode == SDLK_0) {
			if(scale != 1 || tx != 0 || ty != 0) {
				resetFractal();
				renderFractal();
			}
		}
		else if(event.keycode == SDLK_TAB) {
			colorMethod = static_cast<COLOR_METHOD>((colorMethod + 1) % 2);
			renderFractal();
		}
		else if(event.keycode == SDLK_LSHIFT) {
			colorPalette = static_cast<COLOR_PALETTE>((colorPalette + 1) % 3);
			renderFractal();
		}
		else if(event.keycode == SDLK_RETURN) {
			fractalType = static_cast<FRACTAL_TYPE>((fractalType + 1) % 2);
			resetFractal();
			renderFractal();
		}
	}
	void onKeyUp(KeyEvent event) {
		if(event.keycode == SDLK_LEFT || event.keycode == SDLK_a) {keys.left = false;}
		else if(event.keycode == SDLK_UP || event.keycode == SDLK_w) {keys.up = false;}
		else if(event.keycode == SDLK_DOWN || event.keycode == SDLK_s) {keys.down = false;}
		else if(event.keycode == SDLK_RIGHT || event.keycode == SDLK_d) {keys.right = false;}
		else if(event.keycode == SDLK_MINUS) {keys.minus = false;}
		else if(event.keycode == SDLK_EQUALS) {keys.plus = false;}
		else if(event.keycode == SDLK_q) {keys.q = false;}
		else if(event.keycode == SDLK_e) {keys.e = false;}
	}
	void onWindowResize(WindowEvent event) {
		renderFractal();
	} 
	void onMouseWheel(WheelEvent event) {
		if(event.flipped) {event.dy *= -1;}
		if(event.dy > 0) {scale /= zoomFactor;}
		else {scale *= zoomFactor;}
		renderFractal();
	}
	void onMouseMove(MouseEvent event) {
		if(event.down) {
			tx -= event.rx / ((F)ww()) * (size[0]) * scale;
			ty -= event.ry / ((F)wh()) * (size[1]) * scale;
			renderFractal();
		}
	}
	void onMouseDown(MouseButtonEvent event) {
		if(event.which == 3) {
			orbitPoint[0] = MAP(event.x, 0, ww(), params[0] * scale + tx, params[1] * scale + tx);
			orbitPoint[1] = MAP(event.y, 0, wh(), params[2] * scale + ty, params[3] * scale + ty);
			renderFractal();
		}
	}
	void resetFractal() {
		scale = 1;
		tx = 0;
		ty = 0;
		orbitPoint[0] = orbitPoint[1] = 0;
		if(fractalType == MANDELBROT) {
			maxIterations = 100;
			maxDistance = 2.47;
			size[0] = 2.47;
			size[1] = 2;
			params[0] = -2.00;
			params[1] = 0.47;
			params[2] = -1.00;
			params[3] = 1.00;
			orbitPoint[0] = 0.00;
			orbitPoint[1] = 0.00;
		}
		else if(fractalType == JULIA) {
			maxIterations = 100;
			maxDistance = 2.47;
			size[0] = 1;
			size[1] = 1;
			params[0] = -0.7000;
			params[1] = 0.27015;
			params[2] = 2.7907171569;
			params[3] = 0;
			orbitPoint[0] = 0.00;
			orbitPoint[1] = 0.00;
		}
	}
	void renderFractal() {
		// auto start = std::chrono::high_resolution_clock::now();
		int32_t w, h;
		uint8_t* pixels = lockTexture(frame, &w, &h);
		if(pixels != NULL) {
			if(fractalType == JULIA) {
				params[0] = 0.7885 * cos(angle); 
				params[1] = 0.7885 * sin(angle);
			}
			#pragma omp parallel for schedule(dynamic)
			for(uint32_t i = 0; i < w*h; ++i) {
				uint32_t iterations;
				F distance;
				runFractal(w, h, i, iterations, colorMethod, distance);
				F v, max;
				switch(colorMethod) {
					case ITERATIONS: v = iterations; max = maxIterations; break;
					case POINT_ORBIT_TRAP: v = distance; max = maxDistance; break;
				}
				colorPixel(&pixels[i*4], v, max);
			}
			unlockTexture(frame);
			// auto end = std::chrono::high_resolution_clock::now();
			// printf("Took: %lldms\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
		}
	}
	inline void runFractal(int32_t& w, int32_t& h, uint32_t& i, uint32_t& iterations, COLOR_METHOD& colorMethod, F& distance) {
		switch(fractalType) {
			case MANDELBROT: mandelbrot(w, h, i, iterations, colorMethod, distance); break;
			case JULIA: julia(w, h, i, iterations, colorMethod, distance); break;
		}
	}
	inline void colorPixel(uint8_t* pixel, F& value, F& max) {
		switch(colorPalette) {
			case THERMAL: {
				uint8_t* color = thermal[(int)MAP(value, 0, max, 0, THERMAL_LENGTH)];
				pixel[0] = color[0];
				pixel[1] = color[1];
				pixel[2] = color[2];
				break;
			}
			case RGB: {
				uint32_t color = hsla2rgba(MAP(value, 0, max, 0, 359), 1.0, 0.50);
				pixel[0] = (uint8_t)(color >> 24);
				pixel[1] = (uint8_t)(color >> 16);
				pixel[2] = (uint8_t)(color >> 8);
				break;
			}
			case BW: {
				memset(pixel, (uint8_t)round(MAP(value, 0, max, 0, 255)), 3);
				break;
			}
		}
		pixel[3] = 255;
	}

	// Fractals
	void mandelbrot(int32_t& w, int32_t& h, uint32_t& i, uint32_t& iterations, COLOR_METHOD& colorMethod, F& distance) {
		iterations = 0;
		distance = maxDistance;
		F x0 = MAP(i%w, 0, w, params[0] * scale + tx, params[1] * scale + tx);
		F y0 = MAP(i/w, 0, h, params[2] * scale + ty, params[3] * scale + ty);
		F x = 0, y = 0, x2 = 0, y2 = 0, d = 0;
		while(x2 + y2 <= 4 && iterations < maxIterations) {
			y = (x + x) * y + y0;
			x = x2 - y2 + x0;
			x2 = x * x;
			y2 = y * y;

			if(colorMethod == POINT_ORBIT_TRAP) {
				d = sqrt((x2 - (orbitPoint[0] * orbitPoint[0])) + (y2 - (orbitPoint[1] * orbitPoint[1])));
				if(d < distance) {
					distance = d;
				}
			}
			++iterations;
		}
	}
	void julia(int32_t& w, int32_t& h, uint32_t& i, uint32_t& iterations, COLOR_METHOD& colorMethod, F& distance) {
		iterations = 0;
		distance = maxDistance;
		F x = MAP(i%w, 0, w, -params[2] * scale + tx, params[2] * scale + tx);
		F y = MAP(i/w, 0, h, -params[2] * scale + ty, params[2] * scale + ty);
		F x2 = x*x, y2 = y*y, d = 0;
		while(x2 + y2 <= params[2] && iterations < maxIterations) {
			y = (x + x) * y  + params[1];
			x = x2 - y2 + params[0];

			x2 = x * x;
			y2 = y * y;
			if(colorMethod == POINT_ORBIT_TRAP) {
				d = sqrt((x2 - (orbitPoint[0] * orbitPoint[0])) + (y2 - (orbitPoint[1] * orbitPoint[1])));
				if(d < distance) {
					distance = d;
				}
			}
			++iterations;
		}
		// iterations = maxIterations - iterations;
	}
};

// Run Code
int main(int argc, char** argv) {
	FractalVisualizer fv;
	fv.begin();
	return 0;
}



// Helpers
F hue2rgb(F p, F q, F h) {
	if(h < 0.0) {++h;}
	if(h > 1.0) {--h;}
	if(h < 1.0/6.0) {return p + (q-p) * 6.0 * h;}
	else if(h < 1.0/2.0) {return q;}
	else if(h < 2.0/3.0) {return p + (q - p) * (2.0/3.0 - h) * 6.0;}
	return p;
}
uint32_t hsla2rgba(F h, F s, F l, F a) {
	F r, g, b;
	if(s == 0) {r = g = b = l;}
	else {
		h /= 360.0;
		F q = l < 0.5 ? l * (1.0 + s) : (l + s - l * s);
		F p = 2.0 * l - q;
		r = hue2rgb(p, q, h + (1.0/3.0));
		g = hue2rgb(p, q, h);
		b = hue2rgb(p, q, h - (1.0/3.0));
	}
	return ((uint8_t)(r * 255.0) << 24) | ((uint8_t)(g * 255.0) << 16) | ((uint8_t)(b * 255.0) << 8) | (uint8_t)(a * 255.0);
}