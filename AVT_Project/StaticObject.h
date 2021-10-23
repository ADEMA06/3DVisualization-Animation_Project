#pragma once
#include "GameObject.h"

class StaticObject : public GameObject {
public:
	StaticObject(vec3 position) : GameObject(position) {};
};