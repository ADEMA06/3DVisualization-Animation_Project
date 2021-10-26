#ifndef __HUD_H__
#define __HUD_H__

#include "vec.h"
#include <vector>
#include <string>
#include "vsShaderLib.h"
#include "avtFreeType.h"
#include "AVTmathLib.h"

using namespace std;

class HUD {
    vector<string> text;
    vector<vec2> coordinates;

public:
    HUD(){}

    HUD(vector<string> text, vector<vec2> coordinates) {
        this->text = text;
        this->coordinates = coordinates;
    }

    void renderText(VSShaderLib *shader) {
        int m_viewport[4];
        glGetIntegerv(GL_VIEWPORT, m_viewport);
        //viewer looking down at negative z direction
        pushMatrix(MODEL);
        loadIdentity(MODEL);
        pushMatrix(PROJECTION);
        loadIdentity(PROJECTION);
        pushMatrix(VIEW);
        loadIdentity(VIEW);
        ortho(m_viewport[0], m_viewport[0] + m_viewport[2] - 1, m_viewport[1], m_viewport[1] + m_viewport[3] - 1, -1, 1);
        float x, y;
        for (int i = 0; i < this->text.size(); i++) {
            x = this->coordinates.at(i).x;
            y = this->coordinates.at(i).y;
            RenderText(*shader, this->text.at(i), x, y, 1.0f, 1.0f, 1.0f, 1.0f);
        }
        /*RenderText(shaderText, "This is a sample text", 25.0f, 25.0f, 1.0f, 0.5f, 0.8f, 0.2f);
        RenderText(shaderText, "AVT Text Rendering Demo", 440.0f, 570.0f, 0.5f, 0.3, 0.7f, 0.9f);
        */popMatrix(PROJECTION);
        popMatrix(VIEW);
        popMatrix(MODEL);
    }
};


#endif // !__HUD_H__
