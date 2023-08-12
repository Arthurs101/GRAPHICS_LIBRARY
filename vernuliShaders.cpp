#pragma once
#include <vector>
#include "LinealAlgebra.h"
#include "ObjectParser.h"
class Shader {
    std::vector<std::vector<float>> modelMatrix;
    std::vector<std::vector<float>> viewPMatrix;
    std::vector<std::vector<float>> viewMatrix;
    std::vector<std::vector<float>> perspectiveMatrix;
    std::vector<std::vector<float>> transformMatrix;
    Texture txt;
public:
    Shader(std::vector<std::vector<float>> _modelMatrix, Texture _txt = Texture(), std::vector<std::vector<float>> _viewMatrix = {},
        std::vector<std::vector<float>> _perspectiveMatrix = {}, std::vector<std::vector<float>> _viewPMatrix = {}) {
        txt = _txt;
        modelMatrix = _modelMatrix;
        viewMatrix = _viewMatrix;
        perspectiveMatrix = _perspectiveMatrix;
        transformMatrix = multiplyMatrices(multiplyMatrices(multiplyMatrices(viewPMatrix, perspectiveMatrix), viewMatrix), modelMatrix);
    }
    std::vector<float> vertexShader(std::vector<float> vertex) {
        std::vector<float> vt;
        if (transformMatrix.size() > 0) {
            
            vt = multiplyMatrixByVector(transformMatrix, vt);
        }
        else {
            vt = multiplyMatrixByVector(modelMatrix, { vertex[0],vertex[1],vertex[2] ,1 });
        }
        return { vt[0] / vt[3],vt[1] / vt[3],vt[2] / vt[3] };
    }
    std::vector<float> fragmentShader(float u , float v) {
        if (txt.IsValid()) {
         
            //return txt.getColor(u, v);
            return { 200,200,200 };
        }
        else {
            return { 0,0,255 };
        };
    }

    Shader() : modelMatrix({}) , txt(Texture()) {};
};
