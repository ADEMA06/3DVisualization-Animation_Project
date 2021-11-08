#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include <GL/freeglut.h>
#include "vec.h"
#include "AABB.h"

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];


class GameObject {
    vec3 position = {0, 0, 0};

    AABB bounding_box;
 

public:
    GameObject() {}

    GameObject(vec3 position) {
        this->position = position;
    }

    vec3 getPosition(){
        return this->position;
    }

    void setPosition(vec3 position) {
        this->position = position;
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

    MyMesh* setMesh(MyMesh* amesh, float amb[], float diff[], float spec[], float emissive[], float shininess, int texcount, vec3 position) {
        memcpy(amesh->mat.ambient, amb, 4 * sizeof(float));
        memcpy(amesh->mat.diffuse, diff, 4 * sizeof(float));
        memcpy(amesh->mat.specular, spec, 4 * sizeof(float));
        memcpy(amesh->mat.emissive, emissive, 4 * sizeof(float));
        amesh->mat.shininess = shininess;
        amesh->mat.texCount = texcount;
        amesh->position = position;

        return amesh;
    }

    void setBoundingBox(vec3 min_pos, vec3 max_pos) {
        this->bounding_box = AABB(min_pos, max_pos);
    }

    void setBoundingBoxDebug() {
        this->bounding_box.debugMode();
    }

    void updateBoundingBox(vec3 offset) {
        this->bounding_box.updateBoundingBox(offset);
    }

    void updateBoundingBox(float* transformations) {
        this->bounding_box.updateBoundingBox(transformations);
    }
    
    AABB getBoundingBox() {
        return this->bounding_box;
    }

    bool checkCollision(AABB otherBoundingBox) {
        return this->bounding_box.checkCollision(otherBoundingBox);
    }


};

#endif
