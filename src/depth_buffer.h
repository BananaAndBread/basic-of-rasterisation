#pragma once

#include "triangle_rasterization.h"

using namespace linalg::ostream_overloads;

namespace cg
{

    class ZCulling : public TriangleRasterization
    {
    public:
        ZCulling(unsigned short width, unsigned short height, std::string obj_file);
        virtual ~ZCulling();

        void DrawScene();
        void Clear();

    protected:
        virtual void DrawTriangle(face triangle);
        void SetPixel(unsigned short x, unsigned short y, color color, float z);
        std::vector<float> depth_buffer;

        std::vector<color> colors;
        int draw_call_counter;
    };

} // namespace cg
