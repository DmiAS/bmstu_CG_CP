#ifndef VERTEX_SHADER_H
#define VERTEX_SHADER_H
#include "shaders.h"
#include "mat.h"

class VertexShader: public VertexShaderInterface{
public:
    Vertex shade(const Vertex &a,
                 const Mat4x4f& objToWorld,
                 const Camera& cam) override;
    ~VertexShader() override{}
};

#endif // VERTEX_SHADER_H
