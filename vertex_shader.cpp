#include "vertex_shader.h"

Vertex VertexShader::shade(const Vertex &a, const Mat4x4f& objToWorld, const Mat4x4f& projection){
    Vec4f res(a.pos);
    res = res * (objToWorld * projection);
    Vertex output = a;
    output.pos = Vec3f(res.x, res.y, res.z);
    float invZ = 1 / res.w;
    output.u *= invZ;
    output.v *= invZ;
    if (fabs(res.w) > eps )
        output.pos = Vec3f(res.x * invZ, res.y * invZ, res.w);
    return output;
}
