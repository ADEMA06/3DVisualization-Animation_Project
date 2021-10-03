#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include <GL/freeglut.h>
#include "vec.h"

class GameObject {
    vec3 position = {0, 0, 0};
    vec3 rotation = {0, 0, 0};
    vec3 scaling = {1, 1, 1};
    vec3 speed = {0, 0, 0};
    vec3 accel = {0, 0, 0};
 

public:

    GameObject(vec3 position) {
        this->position = position;
    }
 
    GameObject(vec3 position, vec3 speed) {
        this->position = position;
        this->speed = speed;
    }

    GameObject(vec3 position, vec3 speed, vec3 rotation) {
        this->position = position;
        this->speed = speed;
        this->rotation = rotation;
    }

    vec3 getPosition(){
        return this->position;
    }

    void setPosition(vec3 position) {
        this->position = position;
    }

    vec3 getRotation(){
        return this->rotation;
    }

    void setRotation(vec3 rotation) {
        this->rotation = rotation;
    }

    vec3 getScaling(){
        return this->scaling;
    }

    void setScaling(vec3 scaling) {
        this->scaling = scaling;
    }

    vec3 getSpeed(){
        return this->speed;
    }

    void setSpeed(vec3 speed) {
        this->speed = speed;
    }

    vec3 getAccel() {
        return this->accel;
    }

    void setAccel(vec3 accel) {
        this->accel = accel;
    }

    MyMesh setMesh(MyMesh amesh, float amb[], float diff[], float spec[], float emissive[], float shininess, int texcount, vec3 position) {
        memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
        memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
        memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
        memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
        amesh.mat.shininess = shininess;
        amesh.mat.texCount = texcount;
        amesh.position = position;

        return amesh;
    }
};

#endif
