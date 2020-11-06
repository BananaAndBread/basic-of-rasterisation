#include "projections.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <algorithm>
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

cg::ObjParser::ObjParser(std::string filename) : filename(filename)
{
    faces.clear();
}

cg::ObjParser::~ObjParser() { faces.clear(); }

void cg::ObjParser::Parse()
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                                filename.string().c_str(),
                                filename.parent_path().string().c_str(), true);

    if (!warn.empty())
    {
        std::cout << warn << std::endl;
    }

    if (!err.empty())
    {
        std::cerr << err << std::endl;
    }

    if (!ret)
    {
        exit(1);
    }

    for (size_t s = 0; s < shapes.size(); s++)
    {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            int fv = shapes[s].mesh.num_face_vertices[f];

            face current_face;

            for (size_t v = 0; v < fv; v++)
            {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                current_face.vertices[v] = float4(vx, vy, vz, 1);
            }
            index_offset += fv;

            faces.push_back(current_face);
        }
    }
}

const std::vector<cg::face> &cg::ObjParser::GetFaces() { return faces; }

cg::Projections::Projections(unsigned short width, unsigned short height,
                             std::string obj_file)
    : cg::LineDrawing(width, height)
{
    parser = new ObjParser(obj_file);
    parser->Parse();

    cb.world = float4x4{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, -1, -4, 1}};

    float3 look_at{0, 0, 1};
    float3 eye{0, 0, 0};
    float3 up{0, 1, 0};

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

cg::Projections::~Projections() { delete parser; }

void cg::Projections::DrawScene()
{
    unsigned id = 0;
    for (auto face : parser->GetFaces())
    {
        face.id = id++;
        for (unsigned i = 0; i < 3; i++)
        {
            face.vertices[i] = VertexShader(face.vertices[i]);
        }
        Rasterizer(face);
    }
}

float4 cg::Projections::VertexShader(float4 vertex)
{
    return mul(cb.projection, mul(cb.view, mul(cb.world, vertex)));
}

void cg::Projections::Rasterizer(face face)
{
    unsigned x_center = width / 2;
    unsigned y_center = height / 2;
    unsigned scale = std::min(x_center, y_center) - 1;

    for (unsigned i = 0; i < 3; i++)
    {
        face.vertices[i] /= face.vertices[i].w;
        face.vertices[i].x = scale * face.vertices[i].x + x_center;
        face.vertices[i].y = scale * face.vertices[i].y + y_center;
    }

    for (unsigned i = 0; i < 3; i++)
    {
        face.vertices[i].x = std::clamp(face.vertices[i].x, 0.0f, width - 1.0f);
        face.vertices[i].y = std::clamp(face.vertices[i].y, 0.0f, height - 1.0f);
    }

    DrawTriangle(face);
}

void cg::Projections::DrawTriangle(face face)
{
    DrawLine(face.vertices[0].x, face.vertices[0].y, face.vertices[1].x,
             face.vertices[1].y, color(0, 255, 255));
    DrawLine(face.vertices[1].x, face.vertices[1].y, face.vertices[2].x,
             face.vertices[2].y, color(0, 255, 255));
    DrawLine(face.vertices[2].x, face.vertices[2].y, face.vertices[0].x,
             face.vertices[0].y, color(0, 255, 255));
}