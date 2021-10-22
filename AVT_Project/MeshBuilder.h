#pragma once

#include "geometry.h"
#include "VSShaderlib.h"

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

//Shader "global" variables location
extern GLint pvm_uniformId;
extern GLint vm_uniformId;
extern GLint normal_uniformId;
extern GLint lPos_uniformId;

class MeshBuilder {
public:
	MeshBuilder() {}

    /*---------------------------------------------------------------------------------------------
    Puts all attributes in the Mesh Object
    Note: Diffuse is the mesh color*/
    MyMesh setMesh(MyMesh mesh, float amb[], float diff[], 
                                  float spec[], float emissive[], 
                                  float shininess, int texcount, vec3 position) {

        memcpy(mesh.mat.ambient, amb, 4 * sizeof(float));
        memcpy(mesh.mat.diffuse, diff, 4 * sizeof(float));
        memcpy(mesh.mat.specular, spec, 4 * sizeof(float));
        memcpy(mesh.mat.emissive, emissive, 4 * sizeof(float));
        mesh.mat.shininess = shininess;
        mesh.mat.texCount = texcount;
        mesh.position = position;

        return mesh;
    }

    MyMesh* setMesh(MyMesh* amesh, float amb[], float diff[], 
                                   float spec[], float emissive[], 
                                   float shininess, int texcount, vec3 position) {

        memcpy(amesh->mat.ambient, amb, 4 * sizeof(float));
        memcpy(amesh->mat.diffuse, diff, 4 * sizeof(float));
        memcpy(amesh->mat.specular, spec, 4 * sizeof(float));
        memcpy(amesh->mat.emissive, emissive, 4 * sizeof(float));
        amesh->mat.shininess = shininess;
        amesh->mat.texCount = texcount;
        amesh->position = position;

        return amesh;
    }
    /*--------------------------------------------------------------------------------------------*/

    /*----------------------------------------------------------------------------------------------
    Sets shader variables with mesh attributes 
    */
    void setShaders(VSShaderLib shader, MyMesh* mesh) {
        GLint loc;
        loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
        glUniform4fv(loc, 1, mesh->mat.ambient);
        loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
        glUniform4fv(loc, 1, mesh->mat.diffuse);
        loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
        glUniform4fv(loc, 1, mesh->mat.specular);
        loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
        glUniform1f(loc, mesh->mat.shininess);
        loc = glGetUniformLocation(shader.getProgramIndex(), "mat.texCount");
        glUniform1i(loc, mesh->mat.texCount);
    }

	void setShaders(VSShaderLib shader, MyMesh mesh) {
        GLint loc;
        loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
        glUniform4fv(loc, 1, mesh.mat.ambient);
        loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
        glUniform4fv(loc, 1, mesh.mat.diffuse);
        loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
        glUniform4fv(loc, 1, mesh.mat.specular);
        loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
        glUniform1f(loc, mesh.mat.shininess);
        loc = glGetUniformLocation(shader.getProgramIndex(), "mat.texCount");
        glUniform1i(loc, mesh.mat.texCount);
	}
    /*-----------------------------------------------------------------------------------------*/

    /*-------------------------------------------------------------------------------------------
    Computes matrices and draws specified mesh
    */
    void drawMesh(MyMesh mesh, VSShaderLib shader) {
        // send matrices to OGL
        computeDerivedMatrix(PROJ_VIEW_MODEL);
        glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
        glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
        computeNormalMatrix3x3();
        glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

        // Render mesh
        glBindVertexArray(mesh.vao);

        if (!shader.isProgramValid()) {
            printf("Program Not Valid!\n");
            exit(1);
        }
        glDrawElements(mesh.type, mesh.numIndexes, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void drawMesh(MyMesh* mesh, VSShaderLib shader) {
        // send matrices to OGL
        computeDerivedMatrix(PROJ_VIEW_MODEL);
        glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
        glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
        computeNormalMatrix3x3();
        glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

        // Render mesh
        glBindVertexArray(mesh->vao);

        if (!shader.isProgramValid()) {
            printf("Program Not Valid!\n");
            exit(1);
        }

        glDrawElements(mesh->type, mesh->numIndexes, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    /*------------------------------------------------------------------------------------*/
};