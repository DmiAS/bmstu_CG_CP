#include "bary.h"

float interPolateCord(float val1, float val2, float val3, const Vec3f& bary){
    return bary.x * val1 + bary.y * val2 + bary.z * val3;
}

Vec3f baryCentricInterpolation(const Vec3f& a, const Vec3f& b, const Vec3f& c, const Vec3f& bary)
{
    float_t x = interPolateCord(a.x, b.x, c.x, bary);
    float_t y = interPolateCord(a.y, b.y, c.y, bary);
    float_t z = interPolateCord(a.z, b.z, c.z, bary);
    return {x, y, z};
}

float calcBar(Vec3f a, Vec3f b, Vec3f p){
    return (a.y - b.y) * p.x + (b.x - a.x) * p.y + a.x * b.y - b.x * a.y;
}

Vec3f toBarycentric(Vec3f a, Vec3f b, Vec3f c, Vec3f p){


    float numA = calcBar(b, c, p);
    float numB = calcBar(c, a, p);
    float numC = calcBar(a, b, p);

    float denumA = calcBar(b, c, a);
    float denumB = calcBar(c, a, b);
    float denumC = calcBar(a, b, c);

    return {numA / denumA, numB / denumB, numC / denumC};
}
