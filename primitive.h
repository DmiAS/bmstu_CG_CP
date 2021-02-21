#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#include "vec3.h"

class Ray {
public:
  Ray(const Vec3f& origin_, const Vec3f& direction_)
    : origin(origin_)
    , direction(direction_.normalize())
  {}
  Ray(const Ray& other)
    : origin(other.origin)
    , direction(other.direction.normalize())
  {}

public:
  Vec3f origin;
  Vec3f direction;
};

//class Intersection {
//public:
//  Intersection()
//    : q(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity())
//    , n(0.0, 0.0, 0.0)
//    , isLight(false)
//    , lightColour(0.0, 0.0, 0.0)
//    , u(0.0), v(0.0)
//    , pu(0.0, 0.0, 0.0), pv(0.0, 0.0, 0.0)
//    //, t(std::numeric_limits<double>::infinity())
//  {}
//  Intersection(const Intersection& other)
//    : q(other.q)
//    , n(other.n.normalize())
////    , m(other.m)
//    , isLight(other.isLight)
//    , lightColour(other.lightColour)
//    , u(other.u), v(other.v)
//    , pu(other.pu), pv(other.pv)
//    //, t(other.t)
//  {}

//  Vec3f q; // Intersection point
//  Vec3f n; // Surface normal at intersection point
////  std::shared_ptr<const Material> m; // Material properties at intersection point
//  bool isLight; // True if intersection with a light object
//  Vec3f lightColour; // If intersect with light, then this is the colour of the light
//  double u, v; // Parametric coordinates
//  Vec3f pu, pv; // Tangent vectors which form a orthogonal basis with the normal
//  //double t; // Distance from ray's origin along ray's direction vector to intersection point: t*ray.direction + ray.origin
//};

class Primitive {
public:
  virtual ~Primitive();

  virtual bool intersect(const Ray& ray) const
  {
    return false;
  }
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere() = default;
  NonhierSphere(const Vec3f& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();

  virtual bool intersect(const Ray& ray) const;

private:
  Vec3f m_pos;
  float m_radius;
};
#endif // PRIMITIVE_H
