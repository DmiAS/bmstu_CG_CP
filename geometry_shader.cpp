#include "geometry_shader.h"

Vertex GeometryShader::shade(const Vertex &a, const Mat4x4f &projection){
    Vec4f res(a.pos);
    res = res * projection;
    Vertex output = a;
    output.pos = Vec3f(res.x, res.y, res.z);
    float invZ = 1 / res.w;
    output.u *= invZ;
    output.v *= invZ;
    if (fabs(res.w) > eps )
        output.pos = Vec3f(res.x * invZ, res.y * invZ, res.w);
    return output;
}
