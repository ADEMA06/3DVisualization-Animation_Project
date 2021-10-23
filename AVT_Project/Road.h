#ifndef __ROAD_H__
#define __ROAD_H__

#include "Cheerio.h"
#include "Flag.h"
#include <sstream>
#include <vector>
#include <list>

float height_t = 0.0f;
class Road {
	struct MyMesh* mesh;
	float width = 5.0f;
	vec3 last_pos;
	std::vector<Cheerio*> limits;
	std::list<Cheerio*> visible;
	Flag* flag;

public:
	Road(vec3 initial_pos) {
		last_pos = initial_pos;
	}

	void setMesh(MyMesh* mesh) {
		this->mesh = mesh;
	}

	void doNorthRoad(int length) {
		for (int i = 0; i < length; i++) {
			Cheerio* c1 = new Cheerio({ float(i) + last_pos.x, height_t, last_pos.z - width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			Cheerio* c2 = new Cheerio({ float(i) + last_pos.x, height_t, last_pos.z + width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c1);
			limits.push_back(c2);
		}
		last_pos = last_pos + vec3(length, 0, 0);
	}

	void doNorthCurve() {
		for (int i = 0; i < 6; i++) {
			Cheerio* c = new Cheerio({ last_pos.x - width / 2 , height_t, float(i) + last_pos.z }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c);
		}
		Cheerio* c1 = new Cheerio({ last_pos.x + width / 2, height_t, float(last_pos.z) }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		limits.push_back(c1);
		Cheerio* c2 = new Cheerio({ last_pos.x + width / 2, height_t, float(last_pos.z + 1) }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		limits.push_back(c2);
		for (int i = 0; i < 6; i++) {
			Cheerio* c = new Cheerio({ float(i) + last_pos.x - 2.5f, height_t, last_pos.z + 6 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c);
		}
		last_pos = last_pos + vec3(3.5, 0, 3.5);
	}

	void doNorthWestCurve() {
		for (int i = 0; i < 6; i++) {
			Cheerio* c = new Cheerio({ last_pos.x - width / 2 , height_t, -float(i) + last_pos.z }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c);
		}
		Cheerio* c1 = new Cheerio({ last_pos.x + width / 2, height_t, float(last_pos.z) }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		limits.push_back(c1);
		Cheerio* c2 = new Cheerio({ last_pos.x + width / 2, height_t, float(last_pos.z - 1) }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		limits.push_back(c2);
		for (int i = 0; i < 6; i++) {
			Cheerio* c = new Cheerio({ float(i) + last_pos.x - 2.5f, height_t, last_pos.z - 6 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c);
		}
		last_pos = last_pos + vec3(3.5, 0, -3.5);
	}

	void doEastRoad(int length) {
		for (int i = 0; i < length; i++) {
			Cheerio* c1 = new Cheerio({ last_pos.x - width / 2, height_t, float(i) + last_pos.z }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			Cheerio* c2 = new Cheerio({ last_pos.x + width / 2, height_t, float(i) + last_pos.z }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c1);
			limits.push_back(c2);
		}
		last_pos = last_pos + vec3(0, 0, length);
	}

	void doEastCurve() {
		for (int i = 0; i < 6; i++) {
			Cheerio* c = new Cheerio({ float(i) + last_pos.x, height_t, last_pos.z - width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c);
		}
		Cheerio* c1 = new Cheerio({ float(last_pos.x), height_t, last_pos.z + width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		limits.push_back(c1);
		Cheerio* c2 = new Cheerio({ float(last_pos.x + 1), height_t, last_pos.z + width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		limits.push_back(c2);
		for (int i = 0; i < 6; i++) {
			Cheerio* c = new Cheerio({ last_pos.x + 6, height_t, float(i) + last_pos.z - width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c);
		}
		last_pos = last_pos + vec3(3.5, 0, 3.5);
	}

	void doWestCurve() {
		for (int i = 0; i < 6; i++) {
			Cheerio* c = new Cheerio({ float(i) + last_pos.x, height_t, last_pos.z + width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c);
		}
		Cheerio* c1 = new Cheerio({ float(last_pos.x), height_t, last_pos.z - width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		limits.push_back(c1);
		Cheerio* c2 = new Cheerio({ float(last_pos.x + 1), height_t, last_pos.z - width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		limits.push_back(c2);
		for (int i = 0; i < 6; i++) {
			Cheerio* c = new Cheerio({ last_pos.x + 6, height_t, -float(i) + last_pos.z + width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c);
		}
		last_pos = last_pos + vec3(3.5, 0, -3.5);
	}

	void doWestSouthCurve() {
		for (int i = 0; i < 6; i++) {
			Cheerio* c = new Cheerio({ -float(i) + last_pos.x, height_t, last_pos.z + width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c);
		}
		Cheerio* c1 = new Cheerio({ float(last_pos.x), height_t, last_pos.z - width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		limits.push_back(c1);
		Cheerio* c2 = new Cheerio({ float(last_pos.x - 1), height_t, last_pos.z - width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		limits.push_back(c2);
		for (int i = 0; i < 6; i++) {
			Cheerio* c = new Cheerio({ last_pos.x - 6, height_t, -float(i) + last_pos.z + width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c);
		}
		last_pos = last_pos + vec3(-3.5, 0, -3.5);
	}

	void doWestRoad(int length) {
		for (int i = 0; i < length; i++) {
			Cheerio* c1 = new Cheerio({ last_pos.x - width / 2, height_t, -float(i) + last_pos.z }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			Cheerio* c2 = new Cheerio({ last_pos.x + width / 2, height_t, -float(i) + last_pos.z }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c1);
			limits.push_back(c2);
		}
		last_pos = last_pos + vec3(0, 0, -length);
	}

	void doSouthCurve() {
		for (int i = 0; i < 6; i++) {
			Cheerio* c = new Cheerio({ last_pos.x + width / 2 , height_t, float(i) + last_pos.z }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c);
		}
		Cheerio* c1 = new Cheerio({ last_pos.x - width / 2, height_t, float(last_pos.z) }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		limits.push_back(c1);
		Cheerio* c2 = new Cheerio({ last_pos.x - width / 2, height_t, float(last_pos.z + 1) }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		limits.push_back(c2);
		for (int i = 0; i < 6; i++) {
			Cheerio* c = new Cheerio({ -float(i) + last_pos.x + width/2, height_t, last_pos.z + 6 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c);
		}
		last_pos = last_pos + vec3(-3.5, 0, 3.5);
	}

	void doSouthRoad(int length) {
		for (int i = 0; i < length; i++) {
			Cheerio* c1 = new Cheerio({ -float(i) + last_pos.x, height_t, last_pos.z - width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			Cheerio* c2 = new Cheerio({ -float(i) + last_pos.x, height_t, last_pos.z + width / 2 }, vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c1);
			limits.push_back(c2);
		}
		last_pos = last_pos + vec3(-length, 0, 0);
	}


	void draw(VSShaderLib shader, vec3 camera_position, vec3 camera_direction, bool culling) {
		MeshBuilder builder;
		std::vector<vec3> offsets;

		int size = limits.size();
		visible.clear();
		for (int i = 0; i < size; i++) {
			vec3 pos = limits.at(i)->getPosition();
			double dist = sqrt(pow(camera_position.x - pos.x, 2) + pow(camera_position.y - pos.y, 2) + pow(camera_position.z - pos.z, 2));
			vec3 dir = vec3(pos.x - camera_position.x, pos.y - camera_position.y, pos.z - camera_position.z);

			vec3 n_dir = dir.normalize();

			float dot = camera_direction.dot(n_dir);

			if (!culling) {
				dist = 49;
				dot = 0.51;
			}
			if (dist < 50 && dot > 0.5) {

				offsets.push_back(limits.at(i)->getPosition());
				if(dist < 10)
					visible.push_back(limits.at(i));

			}
		}
		builder.setShaders(shader, mesh);
		builder.setShadersInstances(shader, offsets);
		pushMatrix(MODEL);
		builder.drawMeshInstanced(mesh, shader, offsets.size());
		popMatrix(MODEL);
		

		flag->draw(shader);
	}

	void finishLine() {
		flag = new Flag(last_pos, width);
		flag->createFlag();

	}

	std::list<Cheerio*> getVisible() {
		return this->visible;
	}


};

#endif