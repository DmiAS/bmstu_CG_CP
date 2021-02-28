#include "primitive.h"
#include <QtDebug>

#define SIGN(x) (((x) < 0) ? -1 : 1)
size_t quadraticRoots( double A, double B, double C, double roots[2] )
{
    double D;
    double q;

    if( A == 0 ) {
        if( B == 0 ) {
            return 0;
        } else {
            roots[0] = -C/B;
            return 1;
        }
    } else {
        /*  Compute the discrimanant D=b^2 - 4ac */
        D = B*B - 4*A*C;
        if( D < 0 ) {
            return 0;
        } else {
            /* Two real roots */
            q = -( B + SIGN(B)*sqrt(D) ) / 2.0;
            roots[0] = q / A;
            if( q != 0 ) {
                roots[1] = C / q;
            } else {
                roots[1] = roots[0];
            }
            return 2;
        }
    }
}

bool BoundingBox::intersect(const Ray &r) const{
//    float tmin = (min.x - r.origin.x) / r.direction.x;
//    float tmax = (max.x - r.origin.x) / r.direction.x;

//    if (tmin > tmax) std::swap(tmin, tmax);

//    float tymin = (min.y - r.origin.y) / r.direction.y;
//    float tymax = (max.y - r.origin.y) / r.direction.y;

//    if (tymin > tymax) std::swap(tymin, tymax);

//    if ((tmin > tymax) || (tymin > tmax))
//        return false;

//    if (tymin > tmin)
//        tmin = tymin;

//    if (tymax < tmax)
//        tmax = tymax;

//    float tzmin = (min.z - r.origin.z) / r.direction.z;
//    float tzmax = (max.z - r.origin.z) / r.direction.z;

//    if (tzmin > tzmax) std::swap(tzmin, tzmax);

//    if ((tmin > tzmax) || (tzmin > tmax))
//        return false;

//    if (tzmin > tmin)
//        tmin = tzmin;

//    if (tzmax < tmax)
//        tmax = tzmax;

//    return true;

    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    tmin = (bounds[r.sign[0]].x - r.origin.x) * r.invdirection.x;
    tmax = (bounds[1-r.sign[0]].x - r.origin.x) * r.invdirection.x;
    tymin = (bounds[r.sign[1]].y - r.origin.y) * r.invdirection.y;
    tymax = (bounds[1-r.sign[1]].y - r.origin.y) * r.invdirection.y;

    if ((tmin > tymax) || (tymin > tmax))
        return false;
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    tzmin = (bounds[r.sign[2]].z - r.origin.z) * r.invdirection.z;
    tzmax = (bounds[1-r.sign[2]].z - r.origin.z) * r.invdirection.z;

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    return (SIGN(tmin) == SIGN(tmax));


//    return true;
}


Primitive::~Primitive(){}

BoundingBox::~BoundingBox(){}
