#include <chrono>
#include <iostream>

#include "triangle_rasterization.h"

int main(int argc, char *argv[])
{
    try
    {
        cg::TriangleRasterization *render = new cg::TriangleRasterization(1920, 1080, "models/CornellBox-original.obj");
        render->Clear();
        render->DrawScene();
        render->Save("results/triangle_rasterization.png");
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}