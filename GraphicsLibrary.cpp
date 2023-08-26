#include "vernuliGL2.0.cpp"
#include <vector>
#include <iostream>
int main() {
    // Crear una imagen de ejemplo
    int width = 1920; int height = 1080;
    vgImage Img(width, height);
    Texture txt("apocalypse.bmp");
    Img.LightPos = {1,1,0};
    Img.vgPaintBackground(txt);
    Img.vgLookAt({ 1000,0,1400}, { (float)width / 2 -100 ,0,0});
    Img.vgLoad3dObject("SKULL","craneo.OBJ", { (float)width / 2 -100 ,-700,0 }, { 40,40,40 }, {0,0,0}, "craneo.bmp");
    Img.vgLoad3dObject("SKULL", "craneo.OBJ", { (float)width / 2 - 200 ,-700,0 }, { 40,40,40 }, { 0,0,0 }, "craneo.bmp");
    Img.vgLoad3dObject("SKULL", "craneo.OBJ", { (float)width / 2 - 300 ,-700,0 }, { 40,40,40 }, { 0,0,0 }, "craneo.bmp");
    Img.vgLoad3dObject("SKULL", "craneo.OBJ", { (float)width / 2 - 400 ,-700,0 }, { 40,40,40 }, { 0,0,0 }, "craneo.bmp");
    Img.vgLoad3dObject("tank", "E45.OBJ", { 50 ,400,0 }, { 200,200,200 }, { 0,40,-90 }, "E45.bmp");
    Img.vgLoad3dObject("ROBOT", "BOT.obj", {(float)width - 400 ,-700,0}, {40,40,40}, {0,0,-45}, "BOT.bmp");
    Img.vgLoad3dObject("FREIGHT", "Freigther.obj", { 0 ,-700,0 }, { 40,40,40 }, { 0,0,-45 }, "Freighter.bmp");
    RenderInst _bot1 = RenderInst("ROBOT", 't', 4, 0);
    RenderInst _skull = RenderInst("SKULL", 't', 0, 1);
    RenderInst _tank = RenderInst("tank", 't', 3, 0);
    RenderInst _frei = RenderInst("FREIGHT", 't', 1, 0);
    Img.Render3DObjects({_frei,_tank, _skull,_bot1});
    Img.vgWriteBMP("T-Out.bmp");
    return 0;
}