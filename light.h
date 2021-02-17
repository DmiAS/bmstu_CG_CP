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

    void setType(const light_type& light_t){
        t = light_t;
    }
    bool isObject() {return false;}
public:
    light_type t;
    Vec3f direction;
    Vec3f color_intensity;
    Vec3f position;
    float lightning_power;
};
#endif // LIGHT_H
