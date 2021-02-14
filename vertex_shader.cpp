#include "vertex_shader.h"

Vertex VertexShader::shade(const Vertex &a, const Mat4x4f& rotMatrix, const Mat4x4f& objToWorld, const Camera& cam){
    Vec4f res(a.pos);
    res = res * objToWorld;
    res = res * cam.viewMatrix();

    Vertex output = a;
    output.pos = Vec3f(res.x, res.y, res.z);

    auto normal4 = Vec4f(a.normal) * rotMatrix;
    auto normal3 = Vec3f(normal4.x, normal4.y, normal4.z);

    auto diffuse = light_color * std::max(0.f, normal3.normalize() * -dir.normalize()) * intensity;
    auto c = (diffuse + ambient).saturate();
    output.color = output.color.hadamard(c);
    return output;
}
