#include "vertex_shader.h"

Vertex VertexShader::shade(const Vertex &a, const Mat4x4f& rotMatrix, const Mat4x4f& objToWorld, const Camera& cam){
    Vec4f res(a.pos);
    res = res * objToWorld;
    res = res * cam.viewMatrix();

    Vertex output = a;
    output.pos = Vec3f(res.x, res.y, res.z);

    auto normal4 = Vec4f(a.normal) * rotMatrix;
    auto normal3 = Vec3f(normal4.x, normal4.y, normal4.z);
    auto cos_light = std::max(0.f, normal3.normalize() * -dir.normalize());
    const auto d = diffuse.hadamard(intensity * cos_light);
    const auto amb = Vec3f{0.3f, 0.f, 0.f}.hadamard(Vec3f{0.1f, 0.1f, 0.1f});
    const auto c = (d + ambient).saturate();
    output.color = d;
    return output;
}
