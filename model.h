#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <stdint.h>
#include <string>
#include <functional>
#include <memory>
#include "vertex.h"
#include "mat.h"
#include "shaders.h"

const float max_angle = 360, rot_step_x = 15, rot_step_y = 15, rot_step_z = 15;
class Model{

public:

    Model(const std::string& fileName, uint32_t uid_);

    void rotateX(float angle){
        auto step = wrap_angle(angle_x, angle, rot_step_x);
        angle_x = angle;
        rotation_matrix = rotation_matrix * Mat4x4f::RotationX(step);
//        transform_matrix = Mat4x4f::RotationX(angle) * transform_matrix;
    }

    void rotateY(float angle){
        auto step = wrap_angle(angle_y, angle, rot_step_y);
        angle_y = angle;
        rotation_matrix = rotation_matrix * Mat4x4f::RotationY(step);
//        angle_y = wrap_angle(angle_y + angle);
//        transform_matrix = Mat4x4f::RotationY(angle) * transform_matrix;
    }

    void rotateZ(float angle){
        auto step = wrap_angle(angle_z, angle, rot_step_z);
        angle_z = angle;
        rotation_matrix = rotation_matrix * Mat4x4f::RotationZ(step);
//        angle_z = wrap_angle(angle_z + angle);
//        transform_matrix = Mat4x4f::RotationZ(angle) * transform_matrix;
    }

    void shiftX(float dist){
        shift_x += dist;
//        transform_matrix = Mat4x4f::Translation(dist, 0, 0) * transform_matrix;
    }

    void shiftY(float dist){
        shift_y += dist;
//        transform_matrix = Mat4x4f::Translation(0, dist, 0) * transform_matrix;
    }

    void shiftZ(float dist){
        shift_z += dist;
//        transform_matrix = Mat4x4f::Translation(0, 0, dist) * transform_matrix;
    }

    void scaleX(float factor){
        scale_x += factor;
//        transform_matrix = Mat4x4f::ScaleX(factor) * transform_matrix;
    }

    void scaleY(float factor){
        scale_y += factor;
//        transform_matrix = Mat4x4f::ScaleY(factor) * transform_matrix;
    }

    void scaleZ(float factor){
        scale_z += factor;
//        transform_matrix = Mat4x4f::ScaleZ(factor) * transform_matrix;
    }

    Mat4x4f objToWorld() const {
        return Mat4x4f::Scaling(scale_x, scale_y, scale_z) *
               rotation_matrix *
               Mat4x4f::Translation(shift_x, shift_y, shift_z);
    }

    void setColor(const Vec3f& color){
        for (auto& v: vertex_buffer)
            v.color = color;
    }

    uint32_t getUid() const{
        return uid;
    }

private:
    float wrap_angle(float curr_angle, float next_angle, float step){
        if (next_angle < curr_angle)
            step *= -1;
        return step;
    }


public:
    std::vector<uint32_t> index_buffer;
    std::vector<Vertex> vertex_buffer;
    Mat4x4f rotation_matrix = Mat4x4f::Identity();
    Mat4x4f scale_matrix;

private:
    float angle_x = 0.f, angle_y = 0.f, angle_z = 0.f;
    float shift_x, shift_y, shift_z;
    float scale_x = 1.f, scale_y = 1.f, scale_z = 1.f;
    uint32_t uid;
};
#endif // MODEL_H
