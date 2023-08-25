#include "vernuliGL2.0.cpp"
#include <vector>
#include <iostream>
int main() {
    // Crear una imagen de ejemplo
    int width = 4000; int height = 4000;
    vgImage Img(width, height);
    Texture txt("bkg.bmp");
    Img.vgPaintBackground(txt);
    Img.vgLookAt({1000,0,700}, { (float)width / 2 -100 ,0,0});
    Img.vgLoad3dObject("craneo.OBJ", { (float)width / 2 -100 ,(float)height/2,0 }, { 400,400,400 }, {0,0,90}, "craneo.bmp");
    //Img.vgLoad3dObject("BOT.obj", { (float)width / 2 - 100 ,0,0 }, { 40,40,40 }, { 0,0,90 }, "BOT.bmp");
    Img.Render3DObjects(0,0);
    Img.vgWriteBMP("Output.bmp");
    return 0;
}