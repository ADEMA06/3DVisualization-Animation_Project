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
			Cheerio c1 = Cheerio({ float(i), 0, last_pos.z - width/2}, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			Cheerio c2 = Cheerio({ float(i), 0, last_pos.z + width/2}, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			c1.createCheerio();
			c2.createCheerio();
			limits.push_back(c1);
			limits.push_back(c2);
			last_pos = last_pos + vec3(1, 0, 0);
		}
	}

	void draw(VSShaderLib shader, GLint pvm_uniformId, GLint vm_uniformId, GLint normal_uniformId, GLint lPos_uniformId) {
	for(int i = 0; i < limits.size(); i++)
		limits.at(i).drawCheerio(shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);
	}
};

#endif