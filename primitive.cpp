#include "primitive.h"

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

bool NonhierSphere::intersect(const Ray& ray, Intersection& j) const
{

  double roots[2];

  Vec3f v = ray.origin - m_pos;
  auto A = Vec3f::dot(ray.direction, ray.direction);
  auto B = Vec3f::dot(ray.direction * 2.f, v);
  auto C = Vec3f::dot(v, v) - m_radius*m_radius;

  size_t num_roots = quadraticRoots(A, B, C, roots);

  // If there are no roots==no intersections
  // If there is one root, then the ray intersect the sphere on a tangent
  // If there is two roots, then one is the intersection with the sphere from the front as the ray enters the sphere
  // and the other is the intersection through the back as it leaves the sphere
  // For this case we only take the smallest t value since that is the point where the ray enters the sphere which
  // will be the visible point of the sphere. Of course t must not be negative otherwise it is behind the eye point
  if(num_roots > 0)
  {
    // If the ray orginates inside the sphere, then we need to get the max of both roots
    // Otherwise we use the min. If there is only one root, then just use that of course
    // If t is still less than 0 than the sphere is completely behind the ray's origin
    float min = std::min<float>(roots[0], roots[1]);
    float t = (num_roots == 1) ? roots[0] : ((min < 0) ? std::max<float>(roots[0], roots[1]) : min);
    if(t < 0) return false;
    j.q = ray.origin + ray.direction * t;
    j.n = (j.q - m_pos);

    // To calculate the parametric coordinates of the point on the sphere we need to define 3 bivariate functions.
    // For a sphere the spherical coordinate system can be used to define the X, Y, Z coordinates like so:
    // X = r*cos(THETA)*sin(PHI)
    // Y = -r*cos(PHI)
    // Z = -r*sin(THETA)*sin(PHI)
    // Where THETA = atan2(-(z - center.z), x - center.x) and PHI = acos(-(y - center.y) / r)
    // And the parameters u = (THETA + PI) / (2*PI) and v = PHI / PI; u,v E [0, 1]
    float theta = atan2(-j.n.z, j.n.x);
    float phi = acos(-j.n.y / m_radius);

    j.u = (theta + M_PI) / (2 * M_PI);
    j.v = phi / M_PI;

    j.pu = Vec3f(-m_radius*sin(theta)*sin(phi), 0, -m_radius*cos(theta)*sin(phi));
    j.pv = Vec3f(m_radius*cos(theta)*cos(phi), m_radius*sin(phi), -m_radius*sin(theta)*cos(phi));

    //Vector3D pu1, pv1;
    //if(j.n[2] <= j.n[0] && j.n[2] <= j.n[1]) pu1 = Vector3D(-j.n[1], j.n[2], 0.0);
    //else if(j.n[1] <= j.n[0]) pu1 = Vector3D(-j.n[2], 0.0, j.n[0]);
    //else pu1 = Vector3D(0.0, -j.n[2], j.n[1]);
    //pv1 = j.n.cross(pu1);

    return true;
  }

  return false;
}
