#ifndef __ROAD_H__
#define __ROAD_H__

#include "Cheerio.h"
#include "Flag.h"
#include <sstream>
#include <vector>
#include <list>


class Road {
	struct MyMesh* mesh;
	float width = 5.0f;
	vec3 last_pos;
	std::vector<Cheerio*> limits;
	std::list<Cheerio*> visible;
	Flag* flag;
	vec3 direction;

public:
	Road(vec3 initial_pos) {
		last_pos = initial_pos;
		direction = vec3(1.0f, 0.0f, 0.0f);
	}

	void setMesh(MyMesh* mesh) {
		this->mesh = mesh;
	}

	void doRoad(int length) {
		for (int i = 0; i < length; i++) {
			Cheerio* c1 = new Cheerio(vec3(abs(direction.z) == 1.0f ? last_pos.x - width / 2 : last_pos.x , height_t, abs(direction.x) == 1.0f ? last_pos.z - width / 2 : last_pos.z) + direction*float(i),
				vec4(1.0f, 0.874f, 0.0f, 1.0f));
			Cheerio* c2 = new Cheerio(vec3(abs(direction.z) == 1.0f ? last_pos.x + width / 2 : last_pos.x, height_t, abs(direction.x) == 1.0f ? last_pos.z + width / 2 : last_pos.z) + direction*float(i),
				vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c1);
			limits.push_back(c2);
		}
		last_pos = last_pos + direction * length;
	}

	void doRightCurve() {

		float initial_x = direction.x;
		float initial_z = direction.z;

		vec3 curve_front = vec3(direction.x == 1.0f ? last_pos.x : last_pos.x + direction.z * width / 2, height_t, direction.z == 1.0f ? last_pos.z : last_pos.z - direction.x * width/2);
		for (int i = 0; i < 6; i++) {
			Cheerio* c = new Cheerio(curve_front + direction * float(i), vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c);
		}
		vec3 smth = vec3(direction.x == 1.0f ? last_pos.x : last_pos.x - direction.z * width / 2, height_t, direction.z == 1.0f ? last_pos.z : last_pos.z + direction.x * width / 2);
		Cheerio* c1 = new Cheerio(smth, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		limits.push_back(c1);

		if (abs(direction.x) == 1.0f) {
			direction.z = direction.x;
			direction.x = 0.0f;
		}
		else if (abs(direction.z) == 1.0f) {
			direction.x = -direction.z;
			direction.z = 0.0f;
		}
		
		vec3 curve_right = vec3(abs(direction.x) == 1.0f ? last_pos.x - direction.x * width/2: last_pos.x + direction.z * width, height_t, abs(direction.z) == 1.0f ? last_pos.z - direction.z * width/2 : last_pos.z  - direction.x * width);
		for (int i = 0; i < 6; i++) {
			Cheerio* c = new Cheerio(curve_right + direction * float(i), vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c);
		}


		last_pos = last_pos + vec3(abs(direction.x) == 1.0f ? 2.5 * direction.x : 2.5 * initial_x, 0, abs(direction.z) == 1.0f ? 2.5 * direction.z: 2.5 * initial_z) + direction;
	}

	void doLeftCurve() {

		float initial_x = direction.x;
		float initial_z = direction.z;


		vec3 curve_front = vec3(direction.x == 1.0f ? last_pos.x : last_pos.x - direction.z * width / 2, height_t, direction.z == 1.0f ? last_pos.z : last_pos.z + direction.x * width / 2);
		for (int i = 0; i < 6; i++) {
			Cheerio* c = new Cheerio(curve_front + direction * float(i), vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c);
		}

		vec3 smth = vec3(direction.x == 1.0f ? last_pos.x : last_pos.x + direction.z * width / 2, height_t, direction.z == 1.0f ? last_pos.z : last_pos.z - direction.x * width / 2);
		Cheerio* c1 = new Cheerio(smth, vec4(1.0f, 0.874f, 0.0f, 1.0f));
		limits.push_back(c1);

		if (abs(direction.x) == 1.0f) {
			direction.z = -direction.x;
			direction.x = 0.0f;
		}
		else if (abs(direction.z) == 1.0f) {
			direction.x = direction.z;
			direction.z = 0.0f;
		}

		float processed_x = direction.x;
		float processed_z = direction.z;

		printf("%f\n", direction.x);
		vec3 curve_right = vec3(abs(direction.x) == 1.0f ? last_pos.x - direction.x * width / 2 : last_pos.x - direction.z * width, height_t, abs(direction.z) == 1.0f ? last_pos.z - direction.z * width / 2 : last_pos.z + direction.x * width);
		for (int i = 0; i < 6; i++) {
			Cheerio* c = new Cheerio(curve_right + direction * float(i), vec4(1.0f, 0.874f, 0.0f, 1.0f));
			limits.push_back(c);
		}

		printf("%f\n", direction.z);
		last_pos = last_pos + vec3(abs(direction.x) == 1.0f ? 2.5 * direction.x : 2.5 * initial_x, 0, abs(direction.z) == 1.0f ? 2.5 * direction.z : 2.5 * initial_z) + direction;
	}


	void doFinishLine() {
		flag = new Flag(last_pos, width, direction);
		flag->createFlag();
	}


	void draw(VSShaderLib shader, GLint pvm_uniformId, GLint vm_uniformId, GLint normal_uniformId, GLint lPos_uniformId, vec3 camera_position, vec3 camera_direction, bool culling) {
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
				limits.at(i)->drawCheerio(shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId, this->mesh);
				if(dist < 10)
					visible.push_back(limits.at(i));
			}
		}

		flag->draw(shader, pvm_uniformId, vm_uniformId, normal_uniformId, lPos_uniformId);
	}


	std::list<Cheerio*> getVisible() {
		return this->visible;
	}


};

#endif