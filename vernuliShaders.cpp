#pragma once
#include <vector>
#include "LinealAlgebra.h"
#include "ObjectParser.h"
class Shader {
    std::vector<std::vector<float>> modelMatrix;
    std::vector<std::vector<float>> viewPMatrix;
    std::vector<std::vector<float>> viewMatrix;
    std::vector<std::vector<float>> perspectiveMatrix;
    std::vector<std::vector<float>> transformMatrix = {};
    int mode;
    Texture txt;
public:
    Shader(std::vector<std::vector<float>> _modelMatrix, Texture _txt = Texture(), std::vector<std::vector<float>> _viewMatrix = {},
        std::vector<std::vector<float>> _perspectiveMatrix = {}, std::vector<std::vector<float>> _viewPMatrix = {} , int _shadeopt = 0) {
        this-> mode = _shadeopt;
        txt = _txt;
        modelMatrix = _modelMatrix;
        viewMatrix = _viewMatrix;
        perspectiveMatrix = _perspectiveMatrix;
        viewPMatrix = _viewPMatrix;
        if (_viewMatrix.size() > 0 && _perspectiveMatrix.size() > 0 && _viewPMatrix.size() > 0) {
            //transformMatrix = multiplyMatrices(multiplyMatrices(multiplyMatrices(viewPMatrix, perspectiveMatrix), viewMatrix), modelMatrix);
        }
        
    }
    std::vector<float> vertexShader(std::vector<float> vertex) {
        std::vector<float> vt;
        if (transformMatrix.size() > 0) {
            
            vt = multiplyMatrixByVector(transformMatrix, { vertex[0],vertex[1],vertex[2] ,1 });
        }
        else {
            vt = multiplyMatrixByVector(modelMatrix, { vertex[0],vertex[1],vertex[2] ,1 });
        }
        return { vt[0] / vt[3],vt[1] / vt[3],vt[2] / vt[3] };
    }
    std::vector<float> fragmentShader(float u , float v ) {
        switch (this->mode)
        {
            case 0: //renderizado normal
                if (txt.IsValid()) {
                    return txt.getColor(u, v);
                }
                else {
                    return { 0,0,255 };
                };
                break;
            case 1: { //colores negativos
                if (txt.IsValid()) {
                    std::vector<float> _color = txt.getColor(u, v);
                    return {255 - _color[0],255 - _color[1] ,255 - _color[2] };
                }
                else {
                    return { 255,255,0 };
                };
                break;
            }

        default:
            if (txt.IsValid()) {
                return txt.getColor(u, v);
            }
            else {
                return { 0,0,255 };
            };
            break;
        }
       
    }

    Shader() : modelMatrix({}) , txt(Texture()) {};
};
