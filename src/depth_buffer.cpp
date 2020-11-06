#include "depth_buffer.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>

#define INF 1e9

cg::ZCulling::ZCulling(unsigned short width, unsigned short height, std::string obj_file)
    : TriangleRasterization(width, height, obj_file)
{
    depth_buffer = std::vector<float>(static_cast<size_t>(width * height), INF);

    float3 look_at{0, -1, 3};
    float3 eye{0, 0, 6};
    float3 up{0, 1, 0};
    // float3 look_at{0, 0, 3};
    // float3 eye{0, 3, 3};
    // float3 up{0, 0, 1};

    float3 z = normalize(look_at - eye);
    float3 x = normalize(cross(up, z));
    float3 y = cross(z, x);

    float w = 1.0f;
    float h = 1.0f;
    float zn = 1.0f;
    float zf = 10.0f;

    cb.view = float4x4{
        {x.x, x.y, x.z, -dot(x, eye)},
        {y.x, y.y, y.z, -dot(y, eye)},
        {z.x, z.y, z.z, -dot(z, eye)},
        {0, 0, 0, 1}};

    cb.projection = float4x4{
        {2 * zn / w, 0, 0, 0},
        {0, 2 * zn / h, 0, 0},
        {0, 0, zf / (zf - zn), zn * zf / (zn - zf)},
        {0, 0, 1, 0}};
}

cg::ZCulling::~ZCulling() {}

void cg::ZCulling::DrawScene()
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

void cg::ZCulling::Clear()
{
    frame_buffer = std::vector<color>(static_cast<size_t>(width * height));
    depth_buffer = std::vector<float>(static_cast<size_t>(width * height), INF);
}

void cg::ZCulling::DrawTriangle(face triangle)
{
    float x_min = INF, x_max = -INF, y_min = INF, y_max = -INF;
    for (unsigned i = 0; i < 3; i++)
    {
        x_min = std::min(x_min, triangle.vertices[i].x);
        x_max = std::max(x_max, triangle.vertices[i].x);
        y_min = std::min(y_min, triangle.vertices[i].y);
        y_max = std::max(y_max, triangle.vertices[i].y);
    }

    bool ccw = false;
    float d = ccw ? -1 : 1;
    float area = d * EdgeFunction(triangle.vertices[0].xy(), triangle.vertices[1].xy(), triangle.vertices[2].xy());

    for (float x = x_min; x <= x_max; x += 1.f)
    {
        for (float y = y_min; y <= y_max; y += 1.f)
        {
            float2 point{x, y};
            float e0 = d * EdgeFunction(triangle.vertices[0].xy(), triangle.vertices[1].xy(), point);
            float e1 = d * EdgeFunction(triangle.vertices[1].xy(), triangle.vertices[2].xy(), point);
            float e2 = d * EdgeFunction(triangle.vertices[2].xy(), triangle.vertices[0].xy(), point);

            if (e0 >= 0 && e1 >= 0 && e2 >= 0)
            {
                float3 bary{e1 / area, e2 / area, e0 / area};
                float z = bary[0] * triangle.vertices[0].z + bary[1] * triangle.vertices[1].z + bary[2] * triangle.vertices[2].z;
                SetPixel(round(x), round(y), PixelShader(point, bary, triangle.id), z);
            }
        }
    }
}

void cg::ZCulling::SetPixel(unsigned short x, unsigned short y, color color,
                            float z)
{
    if (z < depth_buffer[y * width + x])
    {
        frame_buffer[y * width + x] = color;
        depth_buffer[y * width + x] = z;
    }
}
