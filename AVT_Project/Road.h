#ifndef __ROAD_H__
#define __ROAD_H__

#include "Cheerio.h"
#include <sstream>
#include <vector>


class Road {

	float width = 5.0f;
	vec3 last_pos;
	std::vector<Cheerio> limits;

public:
	Road(vec3 initial_pos) {
		last_pos = initial_pos;
	}

	void doNorthRoad(int length) {
		for (int i = 0; i < length; i++) {
			Cheerio c1 = Cheerio({ float(i) + last_pos.x, 0, last_pos.z - width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			Cheerio c2 = Cheerio({ float(i) + last_pos.x, 0, last_pos.z + width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			c1.createCheerio();
			c2.createCheerio();
			limits.push_back(c1);
			limits.push_back(c2);
		}
		last_pos = last_pos + vec3(length, 0, 0);
	}

	void doNorthCurve() {
		for (int i = 0; i < 6; i++) {
			Cheerio c = Cheerio({ last_pos.x - width / 2 , 0, float(i) + last_pos.z }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			c.createCheerio();
			limits.push_back(c);
		}
		Cheerio c1 = Cheerio({ last_pos.x + width / 2, 0, float(last_pos.z) }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		c1.createCheerio();
		limits.push_back(c1);
		Cheerio c2 = Cheerio({ last_pos.x + width / 2, 0, float(last_pos.z + 1) }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		c2.createCheerio();
		limits.push_back(c2);
		for (int i = 0; i < 6; i++) {
			Cheerio c = Cheerio({ float(i) + last_pos.x - 2.5f, 0, last_pos.z + 6 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			c.createCheerio();
			limits.push_back(c);
		}
		last_pos = last_pos + vec3(3.5, 0, 3.5);
	}

	void doEastRoad(int length) {
		for (int i = 0; i < length; i++) {
			Cheerio c1 = Cheerio({ last_pos.x - width / 2, 0, float(i) + last_pos.z }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			Cheerio c2 = Cheerio({ last_pos.x + width / 2, 0, float(i) + last_pos.z }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			c1.createCheerio();
			c2.createCheerio();
			limits.push_back(c1);
			limits.push_back(c2);
		}
		last_pos = last_pos + vec3(0, 0, length);
	}

	void doEastCurve() {
		for (int i = 0; i < 6; i++) {
			Cheerio c = Cheerio({ float(i) + last_pos.x, 0, last_pos.z - width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			c.createCheerio();
			limits.push_back(c);
		}
		Cheerio c1 = Cheerio({ float(last_pos.x), 0, last_pos.z + width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		c1.createCheerio();
		limits.push_back(c1);
		Cheerio c2 = Cheerio({ float(last_pos.x + 1), 0, last_pos.z + width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		c2.createCheerio();
		limits.push_back(c2);
		for (int i = 0; i < 6; i++) {
			Cheerio c = Cheerio({ last_pos.x + 6, 0, float(i) + last_pos.z - width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			c.createCheerio();
			limits.push_back(c);
		}
		last_pos = last_pos + vec3(3.5, 0, 3.5);
	}

	void doWestCurve() {
		for (int i = 0; i < 6; i++) {
			Cheerio c = Cheerio({ float(i) + last_pos.x, 0, last_pos.z + width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			c.createCheerio();
			limits.push_back(c);
		}
		Cheerio c1 = Cheerio({ float(last_pos.x), 0, last_pos.z - width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		c1.createCheerio();
		limits.push_back(c1);
		Cheerio c2 = Cheerio({ float(last_pos.x + 1), 0, last_pos.z - width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		c2.createCheerio();
		limits.push_back(c2);
		for (int i = 0; i < 6; i++) {
			Cheerio c = Cheerio({ last_pos.x + 6, 0, -float(i) + last_pos.z + width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			c.createCheerio();
			limits.push_back(c);
		}
		last_pos = last_pos + vec3(3.5, 0, -3.5);
	}

	void doWestRoad(int length) {
		for (int i = 0; i < length; i++) {
			Cheerio c1 = Cheerio({ last_pos.x - width / 2, 0, -float(i) + last_pos.z }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			Cheerio c2 = Cheerio({ last_pos.x + width / 2, 0, -float(i) + last_pos.z }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			c1.createCheerio();
			c2.createCheerio();
			limits.push_back(c1);
			limits.push_back(c2);
		}
		last_pos = last_pos + vec3(0, 0, -length);
	}

	void doSouthCurve() {
		for (int i = 0; i < 6; i++) {
			Cheerio c = Cheerio({ last_pos.x + width / 2 , 0, float(i) + last_pos.z }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			c.createCheerio();
			limits.push_back(c);
		}
		Cheerio c1 = Cheerio({ last_pos.x - width / 2, 0, float(last_pos.z) }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		c1.createCheerio();
		limits.push_back(c1);
		Cheerio c2 = Cheerio({ last_pos.x - width / 2, 0, float(last_pos.z + 1) }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		c2.createCheerio();
		limits.push_back(c2);
		for (int i = 0; i < 6; i++) {
			Cheerio c = Cheerio({ -float(i) + last_pos.x + width/2, 0, last_pos.z + 6 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			c.createCheerio();
			limits.push_back(c);
		}
		last_pos = last_pos + vec3(-3.5, 0, 3.5);
	}

	void doSouthRoad(int length) {
		for (int i = 0; i < length; i++) {
			Cheerio c1 = Cheerio({ -float(i) + last_pos.x, 0, last_pos.z - width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			Cheerio c2 = Cheerio({ -float(i) + last_pos.x, 0, last_pos.z + width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			c1.createCheerio();
			c2.createCheerio();
			limits.push_back(c1);
			limits.push_back(c2);
		}
		last_pos = last_pos + vec3(-length, 0, 0);
	}


	void draw(VSShaderLib shader, GLint pvm_uniformId, GLint vm_uniformId, GLint normal_uniformId, GLint lPos_uniformId) {
		for (int i = 0; i < limits.size(); i++)
			limits.at(i).drawCheerio(shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);
	}


};

#endif