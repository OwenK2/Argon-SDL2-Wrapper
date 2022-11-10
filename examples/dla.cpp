/**
 * Example created by Owen Kuhn (author of Argon)
 * To compile run `g++ -std=c++17 -Xpreprocessor -O3 -o dla dla.cpp -lSDL2 -lSDL2_gfx -lomp`
 * Diffusion-limited aggregation (from: https://www.youtube.com/watch?v=Cl_Gjj80gPE)
*/

#define ARGON_INCLUDE_GFX
#include "../src/Argon.h"
#include <vector>
#include <random>

#define MAX_WALKERS 300
#define MAX_FROZEN 5000
#define ITERATIONS_PER_FRAME 1000


#define MAP(x, a, b, c, d) ((float)(c) + (((float)(d) - (float)(c)) / ((float)(b) - (float)(a))) * ((float)(x) - (float)(a)))
float hue2rgb(float p, float q, float h);
uint32_t hsla2rgba(float h, float s, float l, float a = 1.0f);
static std::default_random_engine generator;

struct Walker {
	static std::uniform_real_distribution<float> vectorDist;

	bool frozen = false;
	float x = 0, y = 0;
	uint16_t radius = 5;
	uint32_t color = 0xFFFFFF99;
	
	Walker(int16_t _x, int16_t _y, bool _frozen = false) : x(_x), y(_y), frozen(_frozen) {}

	void draw(Argon& argon) {
		argon.fillCircle(static_cast<int32_t>(x), static_cast<int32_t>(y), radius, frozen ? color : 0xFFFFFFFF);
	}

	void move(int32_t w, int32_t h) {
		x += vectorDist(generator);
		y += vectorDist(generator);
		if(x > w) {x = w;}
		else if(x < 0) {x = 0;}
		if(y > h) {y = h;}
		else if(y < 0) {y = 0;}
	}

	bool checkFrozen(std::vector<Walker>& others) {
		#pragma omp parallel for schedule(dynamic)
		for(Walker& other : others) {
			float d2 = (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y);
			float collideDist = (radius * radius) + (other.radius * other.radius) + (2 * other.radius * radius);
			if(d2 <= collideDist) {
				frozen = true;
				return true;
			}
		}
		return false;
	}

	void setFrozenColor(Argon& argon) {
		float dx = (x - argon.ww() / 2);
		float dy = (y - argon.wh() / 2);
		float d2 = (dx * dx + dy * dy);
		float maxDist2 = (argon.ww() / 2) * (argon.ww() / 2) + (argon.wh() / 2) * (argon.wh() / 2);
		color = hsla2rgba(MAP(d2, 0, maxDist2, 0, 360), 1.0f, 0.5f);
	}
};

std::uniform_real_distribution<float> Walker::vectorDist(-1, 1);


struct DLA : public Argon {
	Texture* frame;

	std::vector<Walker> frozen;
	std::vector<Walker> walking;

	std::uniform_int_distribution<int> perimiterDistribution;

	DLA() : Argon("Diffusion-Limited Aggregation", 800, 800, ARGON_HIGHDPI|ARGON_VSYNC) {
		perimiterDistribution.param(std::uniform_int_distribution<int>::param_type{0, wh() * 2 + ww() * 2});
	}

	void onLoad(WindowEvent event) {
		frozen.reserve(MAX_WALKERS);
		walking.reserve(MAX_WALKERS);
		setAntiAlias(true);

		// Create first seed
		frozen.emplace_back(ww()/2, wh()/2, true);
		frozen[0].setFrozenColor(*this);

		for(int16_t i = 0; i < MAX_WALKERS; ++i) {
			spawnWalker();
		}
	}

	void gameLoop(double dt) {
		for(uint16_t i = 0; i < ITERATIONS_PER_FRAME; ++i) {
			for(auto it = walking.begin(); it != walking.end();) {
				it->move(ww(), wh());
				if(it->checkFrozen(frozen)) {
					it->setFrozenColor(*this);
					frozen.emplace_back((Walker&&) *it);
					it = walking.erase(it);	
					break;
				}
				else {++it;}
			}
		}
		for(Walker& walker : frozen) {
			walker.draw(*this);
		}
		for(Walker& walker : walking) {
			walker.draw(*this);
		}
		while(frozen.size() < MAX_FROZEN && walking.size() < MAX_WALKERS) {spawnWalker();}
	}

	void spawnWalker() {
		uint32_t p = perimiterDistribution(generator);
		int16_t x, y;
		if(p < ww() * 2) {
			x = p % ww();
			y = p < ww() ? 0 : wh();
		}
		else {
			p -= ww() * 2;
			x = p < wh() ? 0 : ww();
			y = p % wh();
		}
		walking.emplace_back(x, y);
	}
};

int main(int argc, char** argv) {
	DLA dla;
	dla.begin();
	return 0;
}


// Color Helpers
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