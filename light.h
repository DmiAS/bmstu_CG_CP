#ifndef LIGHT_H
#define LIGHT_H
#include "model.h"

class Light: public Model{
public:

    enum light_type {
        ambient = 0,
        point,
        directional,
        pointLightSource,
    };

    Light(light_type t_ = ambient, Vec3f direction_ = {0, 0, 1},
          Vec3f color_intensity_ = {0.2f, 0.2f, 0.2f},
          Vec3f position_ = {0, 0, 0}, float lightning_power_ = 1):
        t(t_), direction(direction_), color_intensity(color_intensity_),
        position(position_), lightning_power(lightning_power_){}

    void setType(const light_type& light_t){
        t = light_t;
    }

    virtual bool isObject() override{return false;}

    ~Light(){}

public:
    light_type t;
    Vec3f direction;
    Vec3f color_intensity;
    Vec3f position;
    float lightning_power;
};
#endif // LIGHT_H
