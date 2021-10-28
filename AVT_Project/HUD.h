#ifndef __HUD_H__
#define __HUD_H__

#include "vec.h"
#include <vector>
#include <string>
#include "vsShaderLib.h"
#include "avtFreeType.h"
#include "AVTmathLib.h"

using namespace std;

typedef struct textToRender
{
    string text;
    vec2 coordinates;
    bool toRender;

    textToRender(string text, vec2 coords, bool toRender = true) {
        this->text = text;
        this->coordinates = coords;
        this->toRender = toRender;
    }

}textToRender;

class HUD {

    list<textToRender*> text;

public:
    HUD(){}

    void addText(textToRender* text) {
        printf("%s\n", text->text.c_str());
        this->text.push_back(text);
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
        for (auto text: this->text) {
            if(text->toRender)  RenderText(*shader, text->text, text->coordinates.x, text->coordinates.y, 1.0f, 1.0f, 1.0f, 1.0f);
        }
        /*RenderText(shaderText, "This is a sample text", 25.0f, 25.0f, 1.0f, 0.5f, 0.8f, 0.2f);
        RenderText(shaderText, "AVT Text Rendering Demo", 440.0f, 570.0f, 0.5f, 0.3, 0.7f, 0.9f);
        */popMatrix(PROJECTION);
        popMatrix(VIEW);
        popMatrix(MODEL);
    }
};


#endif // !__HUD_H__
