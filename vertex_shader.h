﻿#ifndef VERTEX_SHADER_H
#define VERTEX_SHADER_H
#include "shaders.h"
#include "mat.h"

class VertexShader: public VertexShaderInterface{
public:
    VertexShader(const Vec3f& dir_ = {0.f, 0.f, 1.f},
                 const Vec3f& diff_ = {0.7f, 0.7f, 0.7f},
                 const Vec3f& ambient_ = {0.1f, 0.1f, 0.1f}):
        dir{dir_}, light_color{diff_}, ambient{ambient_}{}
    Vertex shade(const Vertex &a,
                 const Mat4x4f& rotationMatrix,
                 const Mat4x4f& objToWorld,
                 const Camera& cam) override;
    ~VertexShader() override{}
private:
    Vec3f dir;
    Vec3f light_color;
    Vec3f ambient;
    float intensity = 1.f;
};

#endif // VERTEX_SHADER_H
