#ifndef VERTEX_H
#define VERTEX_H

#include "vec3.h"
class Vertex{
public:
    Vertex(Vec3f pos_, Vec3f normal_, float u_, float v_, Vec3f color_ = {1, 0, 1}):
        pos{pos_}, normal{normal_}, color{color_}, u{u_}, v{v_}{}
public:
    Vec3f pos, normal, color;
    float u, v;
};
#endif // VERTEX_H
