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
    std::vector<float> dirLight;
    int mode;
    Texture txt;
public:
    std::vector<std::vector<float>> Cmatrix;
    Shader(std::vector<std::vector<float>> _modelMatrix, Texture _txt = Texture(), std::vector<std::vector<float>> _viewMatrix = {},
        std::vector<std::vector<float>> _perspectiveMatrix = {}, std::vector<std::vector<float>> _viewPMatrix = {}, int _shadeopt = 0, std::vector<float> _dirLight = {0,1,0}) {
        this-> mode = _shadeopt;
        txt = _txt;
        modelMatrix = _modelMatrix;
        viewMatrix = _viewMatrix;
        perspectiveMatrix = _perspectiveMatrix;
        viewPMatrix = _viewPMatrix;
        dirLight = _dirLight;
        if (_viewMatrix.size() > 0 && _perspectiveMatrix.size() > 0 && _viewPMatrix.size() > 0) {
            transformMatrix = multiplyMatrices(multiplyMatrices(multiplyMatrices(viewPMatrix, perspectiveMatrix), viewMatrix), modelMatrix);
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
    std::vector<float> lightShader(std::vector<float> bcrds , std::vector<std::vector<float>> Tcrds , std::vector<std::vector<float>> Ncrds) {
        // bcrds : coord baricentricas = {u,v,w} 
        // Tcrds : texture coords = {Ta,Tb,Tc} : vectores cada uno
        // Ncrds : Norm coords NA,NB,NC : son vectores
        if (txt.IsValid()) {
            try {
                float Tu = bcrds[0] * Tcrds[0][0] + bcrds[1] * Tcrds[1][0] + bcrds[2] * Tcrds[2][0];
                float Tv = bcrds[0] * Tcrds[0][1] + bcrds[1] * Tcrds[1][1] + bcrds[2] * Tcrds[2][1];
                Tu = static_cast<float> (Tu);
                Tv = static_cast<float> (Tv);
                std::vector<float> _color = txt.getColor(Tu, Tv);
                std::vector<float> Fnormal = { bcrds[0] * Ncrds[0][0] + bcrds[1] * Ncrds[1][0] + bcrds[2] * Ncrds[2][0] ,
                                               bcrds[0] * Ncrds[0][1] + bcrds[1] * Ncrds[1][1] + bcrds[2] * Ncrds[2][1] ,
                                               bcrds[0] * Ncrds[0][2] + bcrds[1] * Ncrds[1][2] + bcrds[2] * Ncrds[2][2]
                };
                float intensity = dot_product(Fnormal, { -dirLight[0] , -dirLight[1] , -dirLight[2] });
                intensity = std::max(intensity, 0.0f);
                intensity = std::min(1.0f, intensity);
                float r = static_cast<float>(intensity * _color[0]);
                float g = static_cast<float>(intensity * _color[1]);
                float b = static_cast<float>(intensity * _color[2]);
                return { r,g,b };
            }
            catch (std::out_of_range& e) {
                return { 255,0,255 };
            }
        }
        return {0,0,0};
    }
    std::vector<float> dragShader(std::vector<float> bcrds, std::vector<std::vector<float>> Tcrds, std::vector<std::vector<float>> Ncrds) {
        // bcrds : coord baricentricas = {u,v,w} 
        // Tcrds : texture coords = {Ta,Tb,Tc} : vectores cada uno
        // Ncrds : Norm coords NA,NB,NC : son vectores
        if (txt.IsValid()) {
            try {
                float Tu = bcrds[0] * Tcrds[0][0] + bcrds[1] * Tcrds[1][0] + bcrds[2] * Tcrds[2][0];
                float Tv = bcrds[0] * Tcrds[0][1] + bcrds[1] * Tcrds[1][1] + bcrds[2] * Tcrds[2][1];
                Tu = static_cast<float> (Tu);
                Tv = static_cast<float> (Tv);
                std::vector<float> Fnormal = { bcrds[0] * Ncrds[0][0] + bcrds[1] * Ncrds[1][0] + bcrds[2] * Ncrds[2][0] ,
                                               bcrds[0] * Ncrds[0][1] + bcrds[1] * Ncrds[1][1] + bcrds[2] * Ncrds[2][1] ,
                                               bcrds[0] * Ncrds[0][2] + bcrds[1] * Ncrds[1][2] + bcrds[2] * Ncrds[2][2]
                };
                float intensity = dot_product(Fnormal, { -dirLight[0] , -dirLight[1] , -dirLight[2] });
                intensity = std::max(intensity, 0.0f);
                intensity = std::min(1.0f, intensity);
                std::vector<float> _color = dragCoeficient(intensity);
                return { static_cast<float>(_color[0]) ,static_cast<float>(_color[1])  ,static_cast<float>(_color[2]) };
            }
            catch (std::out_of_range& e) {
                return { 255,0,255 };
            }
        }
        return { 0,0,0 };
    }
    std::vector<float> dragCoeficient(float intensity) {
        if (intensity >= 0.5f) {
            // Rojo puro
            return { 255.0f, 0.0f, 0.0f };
        }
        else if (intensity <= 0.0f) {
            // Azul puro
            return { 0.0f, 0.0f, 255.0f };
        }
        else if (intensity >= 0.3f) {
            // Interpolación entre rojo y verde
            float factor = (intensity - 0.3f) * 2.0f;
            float r = 255.0f - (factor * 255.0f);
            float g = 255.0f;
            float b = 0.0f;
            return { r, g, b };
        }
        else {
            // Interpolación entre verde y azul
            float factor = intensity * 2.0f;
            float r = 0.0f;
            float g = 255.0f;
            float b = factor * 255.0f;
            return { r, g, b };
        }
    }
    std::vector<float> irradiationShader(std::vector<float> bcrds, std::vector<std::vector<float>> Tcrds, std::vector<std::vector<float>> Ncrds) {
        // bcrds : coord baricentricas = {u,v,w} 
        // Tcrds : texture coords = {Ta,Tb,Tc} : vectores cada uno
        // Ncrds : Norm coords NA,NB,NC : son vectores
        if (txt.IsValid()) {
                float Tu = bcrds[0] * Tcrds[0][0] + bcrds[1] * Tcrds[1][0] + bcrds[2] * Tcrds[2][0];
                float Tv = bcrds[0] * Tcrds[0][1] + bcrds[1] * Tcrds[1][1] + bcrds[2] * Tcrds[2][1];
                Tu = static_cast<float> (Tu);
                Tv = static_cast<float> (Tv);
                std::vector<float> _color = txt.getColor(Tu, Tv);
                std::vector<float> Fnormal = { bcrds[0] * Ncrds[0][0] + bcrds[1] * Ncrds[1][0] + bcrds[2] * Ncrds[2][0] ,
                                               bcrds[0] * Ncrds[0][1] + bcrds[1] * Ncrds[1][1] + bcrds[2] * Ncrds[2][1] ,
                                               bcrds[0] * Ncrds[0][2] + bcrds[1] * Ncrds[1][2] + bcrds[2] * Ncrds[2][2]
                };
                float intensity = dot_product(Fnormal, { -dirLight[0] , -dirLight[1] , -dirLight[2] });
                intensity = std::max(intensity, 0.0f);
                intensity = std::min(1.0f, intensity);
                std::vector<float> cameraForward = { Cmatrix[0][2] ,Cmatrix[1][2] , Cmatrix[2][2] };
                float glowamount = dot_product(Fnormal, cameraForward);
                glowamount = 1 - glowamount;
                glowamount = std::max(glowamount, 0.0f);
                glowamount = std::min(glowamount, 1.0f); //garantizar que no sea menor que 0 ni mayor a 1
                if (glowamount > intensity) {
                    float _g = static_cast<float>(intensity *255.0f);
                    float _b = static_cast<float>( (1 - intensity )* _color[2]);
                    return { 0, _g, _b };
                }
                else {
                    float _r = static_cast<float>(intensity * _color[0]);
                    float _g = static_cast<float>(intensity * _color[1]);
                    float _b = static_cast<float>(intensity * _color[2]);
                    return { 255.0f - _r, 255.0f - _g,  255.0f -  _b };
                }
                
        }
        return { 0,0,0 };
    }
    
    Shader() : modelMatrix({}) , txt(Texture()) {};
};