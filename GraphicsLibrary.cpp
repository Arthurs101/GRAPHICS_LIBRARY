#include "vernuliGL2.0.cpp"
#include <vector>
#include <iostream>
int main() {
    // Crear una imagen de ejemplo
    int width = 1000; int height = 1000;
    vgImage Img(width, height);
    //Texture txt("Yanfei.bmp");
    //Img.vgPaintBackground(txt);
    //Img.vgLookAt({0,-10,10}, { (float)width / 2 ,400,0});
    Img.vgLoad3dObject("BOT.obj", { (float)width / 2 -100 ,(height +0.0f)/2,0 }, { 40,40,40 }, {0,0,90},"BOT.bmp");
    Img.Render3DObjects(0,1);
    Img.vgWriteBMP("twistShader.bmp");
    return 0;
}