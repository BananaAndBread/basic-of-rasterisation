#include <chrono>
#include <iostream>

#include "clear_rendertarget.h"

int main(int argc, char *argv[])
{
    try
    {
        cg::ClearRenderTarget *render = new cg::ClearRenderTarget(1920, 1080);
        render->Clear();
        render->Save("results/clear_rendertarget.png");
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}