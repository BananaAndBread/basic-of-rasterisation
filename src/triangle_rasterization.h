#pragma once

#include "projections.h"

namespace cg
{

    class TriangleRasterization : public Projections
    {
    public:
        TriangleRasterization(unsigned short width, unsigned short height,
                              std::string obj_file);
        virtual ~TriangleRasterization();

        virtual void DrawScene();

    protected:
        virtual color PixelShader(float2 coord, float3 bary, unsigned primitive_id);
        virtual void DrawTriangle(face triangle);
        float EdgeFunction(float2 a, float2 b, float2 x);
    };

} // namespace cg
