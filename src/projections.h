#pragma once

#include <linalg.h>

#include "draw_line.h"
using namespace linalg::aliases;
using namespace linalg::ostream_overloads;

#include <filesystem>
#include <iostream>

namespace cg
{

    struct face
    {
        float4 vertices[3];
        unsigned id;
    };

    class ObjParser
    {
    public:
        ObjParser(std::string filename);
        virtual ~ObjParser();

        void Parse();

        const std::vector<face> &GetFaces();

    protected:
        std::filesystem::path filename;
        std::vector<face> faces;
    };

    struct ConstantBuffer
    {
        float4x4 world;
        float4x4 view;
        float4x4 projection;
    };

    class Projections : public LineDrawing
    {
    public:
        Projections(unsigned short width, unsigned short height,
                    std::string obj_file);
        virtual ~Projections();

        virtual void DrawScene();
        void LookAt(float3 eye, float3 looking_at, float3 up);

    protected:
        ObjParser *parser;
        ConstantBuffer cb;

        float4 VertexShader(float4 vertex);
        void Rasterizer(face face);

        virtual void DrawTriangle(face face);
    };

} // namespace cg
