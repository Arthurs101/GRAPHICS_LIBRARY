#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
class Texture {
private:
    int width;
    int height;
    std::vector<std::vector<std::vector<float>>> pixels;
    bool isValid; // Indica si la textura se cargó correctamente
public:
    Texture() : width(0), height(0), isValid(false) {}


    Texture(const std::string& filename) {
        std::ifstream image(filename, std::ios::binary);
        if (!image) {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            return;
        }

        image.seekg(10);
        int headerSize;
        image.read(reinterpret_cast<char*>(&headerSize), 4);

        image.seekg(18);
        image.read(reinterpret_cast<char*>(&width), 4);
        image.read(reinterpret_cast<char*>(&height), 4);

        image.seekg(headerSize, std::ios::beg);
        pixels.resize(height, std::vector<std::vector<float>>(width, std::vector<float>(3)));

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                unsigned char b, g, r;
                image.read(reinterpret_cast<char*>(&b), 1);
                image.read(reinterpret_cast<char*>(&g), 1);
                image.read(reinterpret_cast<char*>(&r), 1);
                pixels[y][x] = { static_cast<float>(r) / 255.0f,
                                static_cast<float>(g) / 255.0f,
                                static_cast<float>(b) / 255.0f };
            }
        }
    }
    std::vector<float> getColor(float u, float v) {
        if (0 <= u && u < 1 && 0 <= v && v < 1) {
            int x = static_cast<int>(u * width);
            int y = static_cast<int>(v * height);
            if (x >= 0 && x < width && y >= 0 && y < height) {
                return pixels[y][x];
            }
            else {
                return { 255 , 0 ,0 };
            }

            
        }
        else {
            return { 0, 255, 0 };
        }
    }
    bool IsValid() {
        return isValid;
    }
};
class ObjParser {
public:
    std::vector<std::vector<float>> vertices;
    std::vector<std::vector<float>> textcoords;
    std::vector<std::vector<float>> normals;
    std::vector<std::vector<std::vector<float>>> faces;
    std::vector<float> transform;
    std::vector<float> scale;
    std::vector<float> rotation;
    Texture texture;


    ObjParser(const std::string& filename, const std::vector<float>& transformobj, const std::vector<float>& scaleobj, const std::vector<float>& rotationobj , std::string texturefilename = "") {
        if (texturefilename != "") {
            texture = Texture(texturefilename);
        }
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file");
        }
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v") { // Vertices
                std::vector<float> vertex;
                float value;
                while (iss >> value) {
                    vertex.push_back(value);
                }
                vertices.push_back(vertex);
            }
            else if (prefix == "vt") { // Texture Coordinates
                std::vector<float> texcoord;
                float value;
                while (iss >> value) {
                    texcoord.push_back(value);
                }
                textcoords.push_back(texcoord);
            }
            else if (prefix == "vn") { // Normals
                std::vector<float> normal;
                float value;
                while (iss >> value) {
                    normal.push_back(value);
                }
                normals.push_back(normal);
            }
            else if (prefix == "f") { // Faces
                std::vector<std::vector<float>> face;
                std::string vert;
                while (iss >> vert) {
                    std::istringstream viss(vert);
                    std::string v;
                    std::vector<float> vertex;
                    while (std::getline(viss, v, '/')) {
                        vertex.push_back(std::stoi(v));
                    }
                    face.push_back(vertex);
                }
                faces.push_back(face);
            }
        }

        if (transformobj.size() != 3) {
            std::runtime_error("Transform not supported, must be 3 dimensional");
        }
        if (scaleobj.size() != 3) {
            std::runtime_error("Scale not supported, must be 3 dimensional");
        }
        if (rotationobj.size() != 3) {
            std::runtime_error("Rotation not supported, must be 3 dimensional");
        }
        transform = std::move(transformobj);
        scale = std::move(scaleobj);
        rotation = std::move(rotationobj);
    }
  
};




