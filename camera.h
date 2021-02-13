#ifndef CAMERA_H
#define CAMERA_H
#include "vec3.h"
#include "mat.h"

class Camera{
public:
    Camera(float width, float height, Vec3f pos = {0, 0, -5}, Vec3f up = {0, 1, 0}, Vec3f direction = {0, 0, 1},
           float fov = 90, float zn_ = 0.1f, float zf_ = 1000.f): position{pos}, up{up}, direction{direction},
        fov{fov}, zn{zn_}, zf{zf_}{

        aspect_ratio = width / height;

        projectionMatrix = Mat4x4f::ProjectionFOV(fov, aspect_ratio, zn, zf);

    }

    Mat4x4f viewMatrix() const{
        return Mat4x4f::LookAtLH(position, direction, up);
    }



public:
    Vec3f position;
    Vec3f up;
    Vec3f direction;
    float fov = 90;
    float zn;
    float zf;
    float aspect_ratio;
    Mat4x4f projectionMatrix;
};

#endif // CAMERA_H
