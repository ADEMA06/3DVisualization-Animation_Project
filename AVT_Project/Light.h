#pragma once
#include "vec.h"

struct Light {
	vec4 direction;
	vec4 position;
	float cut_off;
	float linear_attenuation;
	float quad_attenuation;
	float const_attenuation;
	int on;
	int type;
};