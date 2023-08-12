#pragma once
#include <vector>
#include "LinealAlgebra.h"
#include "ObjectParser.h"
class Shader {
    std::vector<std::vector<float>> modelMatrix;
    Texture txt;
public:
    Shader(std::vector<std::vector<float>> _modelMatrix, Texture _txt = Texture()) {
        txt = _txt;
        modelMatrix = _modelMatrix;
    }
    std::vector<float> vertexShader(std::vector<float> vertex) {
        std::vector<float> vt = multiplyMatrixByVector(modelMatrix, { vertex[0],vertex[1],vertex[2] ,1 });
        return { vt[0] / vt[3],vt[1] / vt[3],vt[2] / vt[3] };
    }
    std::vector<float> fragmentShader(std::vector<float> txtCrds) {
        if (txt.IsValid()) {
         
            return txt.getColor(txtCrds[0], txtCrds[1]);;
        }
        else {
            return { 0,0,255 };
        };
    }

    Shader() : modelMatrix({}) , txt(Texture()) {};
};
