#ifndef __AABB_H__
#define __AABB_H__

#include "MeshBuilder.h"


/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

class AABB{
    std::vector<vec3> vertices;
    std::vector<vec3> backup;
    struct MyMesh cube;
    vec3 min_pos, max_pos;
    bool colliding;

public:
    AABB(){
    }

    AABB(vec3 min_pos, vec3 max_pos) {
        this->min_pos = min_pos;
        this->max_pos = max_pos;
        vertices.push_back(min_pos);
        vertices.push_back({ min_pos.x, min_pos.y, max_pos.z });
        vertices.push_back({ min_pos.x, max_pos.y, min_pos.z });
        vertices.push_back({ max_pos.x, min_pos.y, min_pos.z });
        vertices.push_back({ max_pos.x, max_pos.y, min_pos.z });
        vertices.push_back({ max_pos.x, min_pos.y, max_pos.z });
        vertices.push_back({ min_pos.x, max_pos.y, max_pos.z });
        vertices.push_back(max_pos);
        backup = vertices;
        colliding = false;
        
    }

    void debugMode() {
        MeshBuilder builder;
        float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
        float diff[] = { 1.0f, 0.0f, 0.0f, 0.8f };
        float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        float shininess = 100.0f;
        int texcount = 0;
        this->cube = createCube();
        this->cube = builder.setMesh(cube, amb, diff, spec, emissive, shininess, texcount, vec3(0.0f, 0.0f, 0.0f));
    }

    bool checkCollision(AABB bounding_box) {
        return ((this->min_pos.x <= bounding_box.max_pos.x && this->max_pos.x >= bounding_box.min_pos.x) &&
            (this->min_pos.y <= bounding_box.max_pos.y && this->max_pos.y >= bounding_box.min_pos.y) &&
            (this->min_pos.z <= bounding_box.max_pos.z && this->max_pos.z >= bounding_box.min_pos.z));
    }

    void updateBoundingBox(float* transformations) {

        //printf("%f\n", transformations[12]);
        vertices = backup;
       
        for (auto& vertex : vertices) {
            float aux[4] = { vertex.x, vertex.y, vertex.z, 1.0f };
            float res[4];
            multMatrixPoint(transformations, aux, res);
            vertex.x = res[0], vertex.y = res[1], vertex.z = res[2];
        }
        
        float min_x = vertices.at(0).x, min_y = vertices.at(0).y, min_z = vertices.at(0).z;
        float max_x = vertices.at(0).x, max_y = vertices.at(0).y, max_z = vertices.at(0).z;

        for (auto& vertex : vertices) {
            if (vertex.x > max_x) max_x = vertex.x;
            if (vertex.y > max_y) max_y = vertex.y;
            if (vertex.z > max_z) max_z = vertex.z;
            if (vertex.x < min_x) min_x = vertex.x;
            if (vertex.y < min_y) min_y = vertex.y;
            if (vertex.z < min_z) min_z = vertex.z;
        }

        this->min_pos = { min_x, min_y, min_z };
        this->max_pos = { max_x, max_y, max_z };

        //printf("max: %f %f\n", max_x, max_z);
        //printf("min: %f %f\n", min_x, min_z);
    }

    void updateBoundingBox(vec3 offset) {

        for (auto& vertex : vertices) {
            vertex = vertex + offset;
        }

        float min_x = vertices.at(0).x, min_y = vertices.at(0).y, min_z = vertices.at(0).z;
        float max_x = vertices.at(0).x, max_y = vertices.at(0).y, max_z = vertices.at(0).z;

        for (auto& vertex : vertices) {
            if (vertex.x > max_x) max_x = vertex.x;
            if (vertex.y > max_y) max_y = vertex.y;
            if (vertex.z > max_z) max_z = vertex.z;
            if (vertex.x < min_x) min_x = vertex.x;
            if (vertex.y < min_y) min_y = vertex.y;
            if (vertex.z < min_z) min_z = vertex.z;
        }

        this->min_pos = { min_x, min_y, min_z };
        this->max_pos = { max_x, max_y, max_z };
    }

    void setMinPos(vec3 min_pos) {
        this->min_pos = min_pos;
    }
    
    void setMaxPos(vec3 max_pos) {
        this->max_pos = max_pos;
    }

    vec3 getMinPos() {
        return this->min_pos;
    }
    
    vec3 getMaxPos() {
        return this->max_pos;
    }

    bool isColliding() {
        return this->colliding;
    }

    void setColliding(bool val) {
        this->colliding = val;
    }

    void draw(VSShaderLib* shader, float* transformations) {
        MeshBuilder builder;
        pushMatrix(MODEL);
        builder.setShaders(shader, cube);
        float scale_x = max_pos.x - min_pos.x;
        float scale_y = max_pos.y - min_pos.y;
        float scale_z = max_pos.z - min_pos.z;
        translate(MODEL, min_pos.x, min_pos.y, min_pos.z);
        scale(MODEL, scale_x, scale_y, scale_z);
        builder.drawMesh(cube, shader);
        popMatrix(MODEL);
    }


};

#endif