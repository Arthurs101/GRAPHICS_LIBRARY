#include "vernuliGL2.0.cpp"
#include <vector>
#include <iostream>
int main() {
    // Crear una imagen de ejemplo
    int width = 4000; int height = 4000;
    vgImage Img(width, height);
    //Texture txt("Yanfei.bmp");
    //Img.vgPaintBackground(txt);
    Img.vgLoad3dObject("Yanfei.obj", { (float)width / 2,(float)height / 2,0 }, { 6000,6000,6000 }, {0,0,0},"Yanfei.bmp");
    Img.Render3DObjects();
    Img.vgWriteBMP("Output.bmp");
    return 0;
}