#ifndef VERTEX_SHADER_H
#define VERTEX_SHADER_H
#include "shaders.h"
#include "mat.h"

const float eps =  1e-7f;
class VertexShader: public VertexShaderInterface{
public:
    Vertex shade(const Vertex &a, const Mat4x4f& objToWorld, const Mat4x4f& projection) override;
    ~VertexShader() override{}
};

#endif // VERTEX_SHADER_H
