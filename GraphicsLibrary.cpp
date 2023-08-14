#include "vernuliGL2.0.cpp"
#include <vector>
#include <iostream>
int main() {
    // Crear una imagen de ejemplo
    int width = 4000; int height = 4000;
    vgImage Img(width, height);
    //Texture txt("Yanfei.bmp");
    //Img.vgPaintBackground(txt);
    //Img.vgLookAt({0,-10,10}, { (float)width / 2 ,400,0});
    Img.vgLoad3dObject("BOT.obj", { (float)width / 2  ,0 + 400,0 }, {  200,200,200 }, {0,0,0},"BOT.bmp");
    Img.Render3DObjects(1);
    Img.vgWriteBMP("Irradiation.bmp");
    return 0;
}