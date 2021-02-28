#ifndef LIGHT_H
#define LIGHT_H
#include "model.h"

const float ambInt = 0.3f;
class Light: public Model{
public:

    enum light_type {
        ambient = 0,
        point,
        directional,
        pointLightSource,
    };

    Light(light_type t_ = ambient, Vec3f color_intensity_ = {ambInt, ambInt, ambInt},
          Vec3f position_ = {0, 0, 0}, float lightning_power_ = 1,
          Vec3f direction_ = {0, 0, 1},
          const std::string& fileName = "", uint32_t uid_ = 0,
          const Vec3f& scale = {1.f, 1.f, 1.f}):
        t(t_), direction(direction_), color_intensity(color_intensity_),
        position(position_), lightning_power(lightning_power_),
        Model(fileName, uid_, scale, position_){}

    void setType(const light_type& light_t){
        t = light_t;
    }

    virtual void shiftX(float dist){
        position.x += dist;
        Model::shiftX(dist);
    }

    virtual void shiftY(float dist){
        position.y += dist;
        Model::shiftY(dist);
    }

    virtual void shiftZ(float dist){
        position.z += dist;
        Model::shiftZ(dist);
    }

    virtual bool isObject() override{return false;}

    virtual ~Light(){}

public:
    light_type t;
    Vec3f direction;
    Vec3f color_intensity;
    Vec3f position;
    float lightning_power;
};
#endif // LIGHT_H
