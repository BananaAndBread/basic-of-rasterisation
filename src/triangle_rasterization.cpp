#include "triangle_rasterization.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <algorithm>
#include <iostream>

#define INF 1e9

cg::TriangleRasterization::TriangleRasterization(unsigned short width, unsigned short height, std::string obj_file)
    : cg::Projections(width, height, obj_file) {}

cg::TriangleRasterization::~TriangleRasterization() {}

void cg::TriangleRasterization::DrawScene()
{
    unsigned id = 0;
    for (auto face : parser->GetFaces())
    {
        face.id = id++;
        for (unsigned i = 0; i < 3; i++)
        {
            face.vertices[i] = VertexShader(face.vertices[i]);
        }
        Rasterizer((face));
    }
}

void cg::TriangleRasterization::DrawTriangle(face triangle)
{
    float x_min = INF, x_max = -INF, y_min = INF, y_max = -INF;
    for (unsigned i = 0; i < 3; i++)
    {
        x_min = std::min(x_min, triangle.vertices[i].x);
        x_max = std::max(x_max, triangle.vertices[i].x);
        y_min = std::min(y_min, triangle.vertices[i].y);
        y_max = std::max(y_max, triangle.vertices[i].y);
    }

    for (unsigned ccw = 0; ccw < 2; ccw++)
    {
        float d = ccw ? -1 : 1;
        float area = d * EdgeFunction(triangle.vertices[0].xy(), triangle.vertices[1].xy(), triangle.vertices[2].xy());

        for (float x = x_min; x <= x_max; x += 1.f)
        {
            for (float y = y_min; y <= y_max; y += 1.f)
            {
                float2 p{x, y};
                float e0 = d * EdgeFunction(triangle.vertices[0].xy(), triangle.vertices[1].xy(), p);
                float e1 = d * EdgeFunction(triangle.vertices[1].xy(), triangle.vertices[2].xy(), p);
                float e2 = d * EdgeFunction(triangle.vertices[2].xy(), triangle.vertices[0].xy(), p);
                if (e0 >= 0 && e1 >= 0 && e2 >= 0)
                {
                    float3 bary{e0 / area, e1 / area, e2 / area};
                    SetPixel(round(x), round(y), PixelShader(p, bary, triangle.id));
                }
            }
        }
    }

    DrawLine(triangle.vertices[0].x, triangle.vertices[0].y, triangle.vertices[1].x, triangle.vertices[1].y, cg::color(255, 0, 0));
    DrawLine(triangle.vertices[1].x, triangle.vertices[1].y, triangle.vertices[2].x, triangle.vertices[2].y, cg::color(255, 0, 0));
    DrawLine(triangle.vertices[2].x, triangle.vertices[2].y, triangle.vertices[0].x, triangle.vertices[0].y, cg::color(255, 0, 0));
}

cg::color cg::TriangleRasterization::PixelShader(float2 coord, float3 bary,
                                                 unsigned primitive_id)
{
    return color(bary[0] * 255, bary[1] * 255, bary[2] * 255);
}

float cg::TriangleRasterization::EdgeFunction(float2 a, float2 b, float2 x)
{
    return (x.x - a.x) * (b.y - a.y) - (x.y - a.y) * (b.x - a.x);
}