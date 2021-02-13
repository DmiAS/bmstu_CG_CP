#include "geometry_shader.h"

Vertex GeometryShader::shade(const Vertex &a, const Mat4x4f &projection){
    Vec4f res(a.pos);
    res = res * projection;
    Vertex output = a;
    output.pos = {res.x, res.y, res.z};

    output.invW = 1 / res.w;
    output.u *= output.invW;
    output.v *= output.invW;

//    output.pos.x *= output.invW;
//    output.pos.y *= output.invW;
//    output.pos.z *= output.invW;

    return output;
}
