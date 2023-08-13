#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include "vernuliShaders.cpp"
#include "ObjectParser.h"
#include "LinealAlgebra.h"




//estructuras
struct Pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    Pixel(unsigned char r, unsigned char g, unsigned char b) {
        red = r;
        green = g;
        blue = b;
    }
    Pixel(std::vector<int> rgb) {
        red = static_cast<unsigned char>(rgb[0]);
        green = static_cast<unsigned char>(rgb[1]);
        blue = static_cast<unsigned char>(rgb[2]);
    }
    Pixel(std::vector<float> rgb) {
        red = static_cast<unsigned char>(rgb[0]);
        green = static_cast<unsigned char>(rgb[1]);
        blue = static_cast<unsigned char>(rgb[2]);
    }
};
struct Image {
    std::vector<std::vector<Pixel>> pixels;
    std::vector<std::vector<float>> zbuffer;
    int width;
    int height;
    Image(int w, int h) : pixels(w, std::vector<Pixel>(h, { 0, 0, 0 })),
        zbuffer(w, std::vector<float>(h, 3.4028235e38)), width(w), height(h) {} //profundidades de zbuffer siendo el valor máximo posible para un float
};

//constantes y variables globales
# define PI 3.14159265

class vgImage {
    int width;
    int height;
    Image imgData;
    std::vector<ObjParser> objects;
    std::vector<Texture> textures;
    Shader currShade;
    std::vector<std::vector<float>> Cmatrix;
    std::vector<std::vector<float>> Viewmatrix;
    std::vector<std::vector<float>> Pmatrix;
    int vpX, vpY, vpWidth, vpHeight;
    std::vector<std::vector<float>> vpMatrix;
    std::vector<float> LightPos = {0,0,0};
    public:
        vgImage(int w, int h) : width(w), height(h), imgData(w, h) {
            glViewport(0, 0, this->width, this->height);
            vgSetCamara();
            vgPerspective();
        }
        /*
        Funcion para escribir el archivo BMP
        */
        void vgWriteBMP(const std::string& filename) {
            std::ofstream file(filename, std::ios::binary);

            // Header
            file.put('B');
            file.put('M');
            int fileSize = 14 + 40 + (width * height * 3);
            file.write(reinterpret_cast<char*>(&fileSize), sizeof(int));
            int reserved = 0;
            file.write(reinterpret_cast<char*>(&reserved), sizeof(int));
            int dataOffset = 14 + 40;
            file.write(reinterpret_cast<char*>(&dataOffset), sizeof(int));

            // InfoHeader
            int infoHeaderSize = 40;
            file.write(reinterpret_cast<char*>(&infoHeaderSize), sizeof(int));
            file.write(reinterpret_cast<char*>(&width), sizeof(int));
            file.write(reinterpret_cast<char*>(&height), sizeof(int));
            short planes = 1;
            file.write(reinterpret_cast<char*>(&planes), sizeof(short));
            short bitsPerPixel = 24;
            file.write(reinterpret_cast<char*>(&bitsPerPixel), sizeof(short));
            int compression = 0;
            file.write(reinterpret_cast<char*>(&compression), sizeof(int));
            int imageSize = width * height * 3;
            file.write(reinterpret_cast<char*>(&imageSize), sizeof(int));
            int horizontalResolution = 0;
            file.write(reinterpret_cast<char*>(&horizontalResolution), sizeof(int));
            int verticalResolution = 0;
            file.write(reinterpret_cast<char*>(&verticalResolution), sizeof(int));
            int colors = 0;
            file.write(reinterpret_cast<char*>(&colors), sizeof(int));
            int importantColors = 0;
            file.write(reinterpret_cast<char*>(&importantColors), sizeof(int));

            // Color table
            for (int y = 0; y < height; y++) { // BMP files are stored upside down
                for (int x = 0; x < width; x++) {
                    file.put(imgData.pixels[x][y].blue);
                    file.put(imgData.pixels[x][y].green);
                    file.put(imgData.pixels[x][y].red);
                }
            }

            file.close();
        }
        
        /*colorear ub pixel*/
        void vgPoint(int x, int y, Pixel clr = { 255,255,255 }) {
            if (0 <= x && x < width && 0 <= y && y < height) {
                imgData.pixels[x][y].red = clr.red;
                imgData.pixels[x][y].green = clr.green;
                imgData.pixels[x][y].blue = clr.blue;
            }
        }
        
        /*
        Dibujar linea
        */
        void vgCreateLine(std::vector<float> v0, std::vector<float> v1, Pixel clr = { 255,255,255 }) {
            int x0 = static_cast<int>(v0[0]);
            int x1 = static_cast<int>(v1[0]);
            int y0 = static_cast<int>(v0[1]);
            int y1 = static_cast<int>(v1[1]);

            // Si el punto 0 es igual al punto 1, solo dibuja un punto
            if (x0 == x1 && y0 == y1) {
                vgPoint(x0, y0, clr);
                return;
            }

            int dy = std::abs(y1 - y0);
            int dx = std::abs(x1 - x0);

            bool steep = dy > dx;

            // Si la línea tiene pendiente mayor a 1 o menor a -1
            // intercambiamos las x por las y, y se dibuja la línea
            // de manera vertical en lugar de horizontal
            if (steep) {
                std::swap(x0, y0);
                std::swap(x1, y1);
            }

            // Si el punto inicial en x es mayor que el punto final en x,
            // intercambiamos los puntos para siempre dibujar de
            // izquierda a derecha
            if (x0 > x1) {
                std::swap(x0, x1);
                std::swap(y0, y1);
            }

            dy = std::abs(y1 - y0);
            dx = std::abs(x1 - x0);

            float offset = 0;
            float limit = 0.5;
            float m = static_cast<float>(dy) / dx;
            int y = y0;

            for (int x = x0; x <= x1; x++) {
                if (steep) {
                    // Dibujar de manera vertical
                    vgPoint(y, x, clr);
                }
                else {
                    vgPoint(x, y, clr);
                }

                offset += m;
                if (offset >= limit) {
                    if (y0 < y1) {
                        y += 1;
                    }
                    else {
                        y -= 1;
                    }
                    limit += 1;
                }
            }
        };
        
        void vgCreatePoligon(std::vector<std::vector<float>> vertices, bool fill = false, Pixel clr = { 255,255,255 }){
            if (vertices.size() < 2) {
                std::cout << "Se requieren al menos dos coordenadas para combinar." << std::endl;
                return;
            };
            
            std::vector<std::vector<float>> combined = vertices;
            // Combina el último punto con el primer punto
            std::vector<float> lastPoint = combined.back();
            std::vector<float> firstPoint = combined.front();
            vgCreateLine(lastPoint, firstPoint,clr);

            // Combina cada par de coordenadas consecutivas
            for (size_t i = 0; i < combined.size() - 1; ++i) {
                std::vector<float> currentPoint = combined[i];
                std::vector<float> nextPoint = combined[i + 1];
                vgCreateLine(currentPoint, nextPoint, clr);
            }
            if (fill) {
                //obtener los limites x, y de los vertices que conforman el poligono
                float minX = std::numeric_limits<float>::max();
                float maxX = std::numeric_limits<float>::min();
                for (const auto& vertex : vertices) {
                    minX = std::min(minX, vertex[0]);
                    maxX = std::max(maxX, vertex[0]);
                }

                float minY = std::numeric_limits<float>::max();
                float maxY = std::numeric_limits<float>::min();
                for (const auto& vertex : vertices) {
                    minY = std::min(minY, vertex[1]);
                    maxY = std::max(maxY, vertex[1]);
                }

                int vertexS = vertices.size();

                for (float y = minY; y <= maxY; y++) {
                    std::vector<float> intersecciones = {};
                    //calcular las intersecciones de los vertices
                    for (int i = 0; i < vertexS; i++) {
                        int k = (i + 1) % vertexS;
                        std::vector<float> V0 = vertices[i];
                        std::vector<float> V1 = vertices[k];
                        //verificar alguna interseccion del vertice con la fila Y actual
                        if (V0[1] < y and V1[1] >= y or V1[1] < y and V0[1] >= y) {
                            float iP = V0[0] + (y - V0[1]) / (V1[1] - V0[1]) * (V1[0] - V0[0]);
                            intersecciones.push_back(iP);
                        }
                    }
                    //ordenar las intersecciones
                    std::sort(intersecciones.begin(), intersecciones.end());
                    //pintar cada interseccion 
                    for (int i = 0; i < intersecciones.size(); i += 2) {
                        float x0 = intersecciones[i];
                        float x1 = std::min(intersecciones[i + 1], maxX);
                        for (int x = x0; x <= x1; x += 1) {
                            vgPoint(x, y, clr);
                        }
                    }
                }
            }
        }
        
        /*cargar objetos 3d*/
        void vgLoad3dObject(const std::string& filename, const std::vector<float>& transformobj = { 0,0,0 }, const std::vector<float>& scaleobj = { 1,1,1 }, const std::vector<float>& rotationobj = { 0,0,0 }, std::string texturefilename = "") {
            objects.push_back(ObjParser(filename, transformobj, scaleobj, rotationobj, texturefilename));
        }

        void CreateBCTriangle(const std::vector<float>& A, const std::vector<float>& B, const std::vector<float>& C, bool multicolor = false, const Pixel& color = {255,255,255}) {
            
            int minX = std::round(std::min({ A[0], B[0], C[0] }));
            int maxX = std::round(std::max({ A[0], B[0], C[0] }));
            int minY = std::round(std::min({ A[1], B[1], C[1] }));
            int maxY = std::round(std::max({ A[1], B[1], C[1] }));
            
            // Obtener el área del triángulo (usada para determinar la escala del gradiente de colores)
            float areaABC = abs((A[0] * (B[1] - C[1]) + B[0] * (C[1] - A[1]) + C[0] * (A[1] - B[1])));

            // Bucle para iterar sobre cada píxel dentro del triángulo
            for (int x = minX; x <= maxX; x++) {
                for (int y = minY; y <= maxY; y++) {
                    std::vector<float> P = { static_cast<float>(x), static_cast<float>(y) };
                    std::vector<float> baryCoords = barycentricCoords(A, B, C, P);

                    // Si las coordenadas baricéntricas son válidas, colorear el píxel 
                    if (baryCoords[0] >= 0 && baryCoords[0] <= 1 && baryCoords[1] >= 0 && baryCoords[1] <= 1 && baryCoords[2] >= 0 && baryCoords[2] <= 1) {
                        float u = baryCoords[0];
                        float v = baryCoords[1];
                        float w = baryCoords[2];
                        unsigned char r;
                        unsigned char g ;
                        unsigned char b ;
                        float z = u * A[2] + v * B[2] + w * C[2];
                        if (x < imgData.width && y < imgData.height && x >= 0 && y >= 0) {
                            if (z < imgData.zbuffer[x][y]) {
                                this->imgData.zbuffer[x][y] = z;
                                // Calcular los valores de color del píxel mediante interpolación lineal
                                if (multicolor) {
                                    r = static_cast<unsigned char>(color.red * u);
                                    g = static_cast<unsigned char>(color.green * v);
                                    b = static_cast<unsigned char>(color.blue * w);
                                }
                                else {
                                    r = color.red;
                                    g = color.green;
                                    b = color.blue;
                                }
                                // Asignar los valores de color al píxel en el array de pixeles
                                vgPoint(x, y, { r,g,b });
                            }
                        }
                    }
                }
            }
        }

        void CreateBCTriangle(const std::vector<float>& A, const std::vector<float>& B, const std::vector<float>& C, 
            const std::vector<float>& vtA, const std::vector<float>& vtB, const std::vector<float>& vtC,
            const std::vector<float>& vnA, const std::vector<float>& vnB, const std::vector<float>& vnC) {

            int minX = std::round(std::min({ A[0], B[0], C[0] }));
            int maxX = std::round(std::max({ A[0], B[0], C[0] }));
            int minY = std::round(std::min({ A[1], B[1], C[1] }));
            int maxY = std::round(std::max({ A[1], B[1], C[1] }));

            // Obtener el área del triángulo (usada para determinar la escala del gradiente de colores)
            float areaABC = abs((A[0] * (B[1] - C[1]) + B[0] * (C[1] - A[1]) + C[0] * (A[1] - B[1])));

            // Bucle para iterar sobre cada píxel dentro del triángulo
            for (int x = minX; x <= maxX; x++) {
                for (int y = minY; y <= maxY; y++) {
                    std::vector<float> P = { static_cast<float>(x), static_cast<float>(y) };
                    std::vector<float> baryCoords = barycentricCoords(A, B, C, P);

                    // Si las coordenadas baricéntricas son válidas, colorear el píxel 
                    if (baryCoords[0] >= 0 && baryCoords[0] <= 1 && baryCoords[1] >= 0 && baryCoords[1] <= 1 && baryCoords[2] >= 0 && baryCoords[2] <= 1) {
                        float u = baryCoords[0];
                        float v = baryCoords[1];
                        float w = baryCoords[2];

                        float z = u * A[2] + v * B[2] + w * C[2];
                        if (x < imgData.width && y < imgData.height && x >= 0 && y >= 0) {
                            if (z < imgData.zbuffer[x][y]) {
                                imgData.zbuffer[x][y] = z;
                                // Calcular los valores de color del píxel mediante interpolación lineal
                                //coordenadas del texto
                                float u0 = static_cast<float>(u * vtA[0] + v * vtB[0] + w * vtC[0]);
                                float v0 = static_cast<float> (u * vtA[1] + v * vtB[1] + w * vtC[1]);
                                // Asignar los valores de color al píxel en el array de pixeles
                                //std::vector<float> color = currShade.fragmentShader(u0,v0);
                                std::vector<float> color = currShade.lightShader(
                                    {u,v,w},
                                    { vtA, vtB, vtC },
                                    { vnA, vnB, vnC }
                                );
                                unsigned char r, g, b;
                                r = static_cast<unsigned char>(color[0]);
                                g = static_cast<unsigned char>(color[1]);
                                b = static_cast<unsigned char>(color[2]);
                                vgPoint(x, y, { r, g, b });
                            }
                        }
                    }
                }
            }
        }

        void Render3DObjects(int shade_opt = 0 ,char PRIMTYPE = 't') {
            for (int i = 0; i < objects.size(); i++) {
                    std::vector<std::vector<float>> M = Generate3DObjectMatrix(objects[i].transform, objects[i].scale, objects[i].rotation);
                    std::vector<std::vector<float>> shaded_vertices = {};
                    std::vector<std::vector<float>> texture_coords = {};
                    std::vector<std::vector<float>> face_normals = {};
                    int faces_ = objects[i].faces.size();
                    bool isValidTXT = objects[i].texture.IsValid();
                    currShade = Shader(M,objects[i].texture,this->Viewmatrix,this->Pmatrix,this->vpMatrix,shade_opt);
                   
                    for (int face = 0; face < faces_; face++) {
                        std::vector<float> v0 = objects[i].vertices[objects[i].faces[face][0][0] - 1];
                        std::vector<float> v1 = objects[i].vertices[objects[i].faces[face][1][0] - 1];
                        std::vector<float> v2 = objects[i].vertices[objects[i].faces[face][2][0] - 1];
                        //texture coords
                        std::vector<float> vt0 = objects[i].textcoords[objects[i].faces[face][0][1] - 1];
                        std::vector<float> vt1 = objects[i].textcoords[objects[i].faces[face][1][1] - 1];
                        std::vector<float> vt2 = objects[i].textcoords[objects[i].faces[face][2][1] - 1];
                        //face normals
                        std::vector<float> vn0 = objects[i].normals[objects[i].faces[face][0][2] - 1];
                        std::vector<float> vn1 = objects[i].normals[objects[i].faces[face][1][2] - 1];
                        std::vector<float> vn2 = objects[i].normals[objects[i].faces[face][2][2] - 1];

                        shaded_vertices.push_back(currShade.vertexShader(v0));
                        shaded_vertices.push_back(currShade.vertexShader(v1));
                        shaded_vertices.push_back(currShade.vertexShader(v2));

                        if (isValidTXT) {
                            texture_coords.push_back(vt0);
                            texture_coords.push_back(vt1);
                            texture_coords.push_back(vt2);
                        }

                        face_normals.push_back(vn0);
                        face_normals.push_back(vn1);
                        face_normals.push_back(vn2);

                        if (objects[i].faces[face].size() == 4) {
                            std::vector<float> v3 = objects[i].vertices[objects[i].faces[face][3][0] - 1];;
                            std::vector<float> vt3 = objects[i].textcoords[objects[i].faces[face][3][1] - 1];;
                            std::vector<float> vn3 = objects[i].normals[objects[i].faces[face][2][2] - 1];
                            
                            shaded_vertices.push_back(currShade.vertexShader(v0));
                            shaded_vertices.push_back(currShade.vertexShader(v2));
                            shaded_vertices.push_back(currShade.vertexShader(v3));
                            
                            if (isValidTXT) {
                                texture_coords.push_back(vt0);
                                texture_coords.push_back(vt2);
                                texture_coords.push_back(vt3);
                            }

                            face_normals.push_back(vn3);
                        }
                    }
                    if (isValidTXT) {
                         Assemble(shaded_vertices, texture_coords ,face_normals, PRIMTYPE);
                    }
                    else {
                        Assemble(shaded_vertices);
                    }


            }
            
        }
        
        void vgPaintBackground(Texture txt) {
            for (int x = 0; x < width; x++) {
                for (int y = 0; y < height; y++) {
                    float u = static_cast<float>(x) / width;
                    float v = static_cast<float>(y) / height;

                    unsigned char r, g, b;
                    std::vector<float> color = txt.getColor(u, v);
                    r = static_cast<unsigned char>(color[0]);
                    g = static_cast<unsigned char>(color[1]);
                    b = static_cast<unsigned char>(color[2]);

                    vgPoint(x, y, { r, g, b });
                }
            }
        }

        void vgSetCamara(std::vector<float> trasnlate = {0,0,0} , std::vector<float> rotate = { 0,0,0 }) {
            this->Cmatrix = Generate3DObjectMatrix(trasnlate, {1,1,1} ,rotate);
            this->Viewmatrix = matriz_inversa(this->Cmatrix);
        }

        void vgLookAt(std::vector<float> Cpos = { 0,0,0 } , std::vector<float> Eyepos = { 0,0,0 }) {
            std::vector<float> worldUp = { 0,1,0 };
            std::vector<float> forward = subtract_arrays(Cpos, Eyepos);
            forward = normalizar_vector(forward);

            std::vector<float> rigth = producto_cruz(worldUp, forward);
            rigth = normalizar_vector(rigth);

            std::vector<float> up = producto_cruz(forward, rigth);
            up = normalizar_vector(up);

            this->Cmatrix = { { rigth[0], up[0], forward[0], Cpos[0] },
            { rigth[1], up[1], forward[1],  Cpos[1] },
            { rigth[2], up[2], forward[2],  Cpos[2] },
            { 0, 0, 0, 1 } };

            this->Viewmatrix = matriz_inversa(this->Cmatrix);
        }

        void vgPerspective(float fov = 60 , float n = 0.1 , float f = 1000 ) {
            float aspect_ratio = static_cast<float> (width) / height;
            float t = std::tan((fov * PI / 180) / 2) * n;
            float r = t * aspect_ratio;
            this->Pmatrix = { {n / r , 0,0,0} , 
                            {0,n/t,0,0},
                            {0, 0, -(f + n) / (f - n), -2 * f * n / (f - n)},
                            {0, 0, -1, 0} };
        }

        void glViewport(int x, int y, int width, int height) {
            vpX = x;
            vpY = y;
            vpWidth = width;
            vpHeight = height;

            vpMatrix = {
                {vpWidth / 2.0f, 0, 0, vpX + vpWidth / 2.0f},
                {0, vpHeight / 2.0f, 0, vpY + vpHeight / 2.0f},
                {0, 0, 0.5f, 0.5f},
                {0, 0, 0, 1}
            };
        }
    private:

            void Assemble(std::vector<std::vector<float>>& vertices, std::vector<std::vector<float>>& text_cords, std::vector<std::vector<float>>& face_normals,char PRIMTYPE = 't') {
                switch (PRIMTYPE)
                {
                case 't':
                    for (int vtx = 0; vtx < vertices.size(); vtx += 3) {
                        CreateBCTriangle(vertices[vtx], vertices[vtx + 1], vertices[vtx + 2], 
                                        text_cords[vtx], text_cords[vtx +1 ], text_cords[vtx + 2 ],
                                        face_normals[vtx], face_normals[vtx + 1], face_normals[vtx + 2]);
                    };
                    break;

                default:
                    std::runtime_error("Unknown PRIMTYPE, try; p for point, t for triangles , l: line , s: square");
                    break;
                }

            }
            
            void Assemble(std::vector<std::vector<float>>& vertices, int thickness = 1, char PRIMTYPE = 't') {
                switch (PRIMTYPE)
                {
                case 't':
                    for (int vtx = 0; vtx < vertices.size(); vtx += 3) {
                        CreateBCTriangle(vertices[vtx], vertices[vtx + 1], vertices[vtx + 2], true);
                    };
                    break;

                default:
                    std::runtime_error("Unknown PRIMTYPE, try; p for point, t for triangles , l: line , s: square");
                    break;
                }

            }

            std::vector<std::vector<float> > Generate3DObjectMatrix(const std::vector<float>& transformobj = { 0,0,0 }, const std::vector<float>& scaleobj = { 1,1,1 }, const std::vector<float>& rotationobj = { 0,0,0 }) {
               
                std::vector<std::vector<float> > Mt = { {1,0,0,transformobj[0]},
                                                        {0,1,0,transformobj[1]},
                                                        {0,0,1,transformobj[2]},
                                                        {0,0,0,1} };
                std::vector<std::vector<float> > Ms = { {scaleobj[0],0,0,0},
                                                        {0,scaleobj[1],0,0},
                                                        {0,0,scaleobj[2],0},
                                                        {0,0,0,1} };
                std::vector<std::vector<float> > Mr = Generate3DAnglesMatrix(rotationobj[0], rotationobj[1], rotationobj[2]);
                std::vector<std::vector<float> > tmp = multiplyMatrices(Mt, Mr);
                return multiplyMatrices(tmp,Ms);
            };

            std::vector<std::vector<float> > Generate3DAnglesMatrix(float pitch , float yaw , float roll) {
                pitch *= PI/180;
                roll +=  PI / 180;
                yaw += PI / 180;
                std::vector<std::vector<float> > Rx = { { 1,0,0,0 }, 
                                                        { 0,(float)cos(pitch),(float)-sin(pitch),0}, 
                                                        {0,(float)sin(pitch),(float)cos(pitch),0}, 
                                                        {0,0,0,1} };
                std::vector<std::vector<float> > Ry = { { (float)cos(roll) , 0, (float)sin(roll), 0 },
                                                        { 0        , 1, 0       , 0 },
                                                        { (float)-sin(roll), 0, (float)cos(roll), 0 },
                                                        { 0        , 0, 0       , 1 }};
                std::vector<std::vector<float> > Rz = { { (float)cos(yaw),-(float)sin(yaw),0,0 }, 
                                                        { (float)sin(yaw),(float)cos(yaw),0,0 },
                                                        { 0,0,1,0 }, 
                                                        { 0,0,0,1 } };
                return multiplyMatrices(multiplyMatrices(Rx, Ry), Rz);
            }

};
