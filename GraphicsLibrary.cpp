#include "vernuliGL2.0.cpp"
#include <vector>
#include <iostream>
int main() {
    // Crear una imagen de ejemplo
    int width = 2048; int height = 2048;
    vgImage Img(width, height);
    Texture txt("Yanfei.bmp");
    Img.vgPaintBackground(txt);
    Img.CreateBCTriangle({ 0,0,0 }, { (float)width / 2, (float)height - 1 , 0 }, {(float)width-1 , 0 ,0},true);
    Img.vgWriteBMP("Output.bmp");
    return 0;
}