#ifndef VEC3_H
#define VEC3_H
#include <math.h>

template <typename T>
class Vec3{
public:
    Vec3(T x_, T y_, T z_ = 0): x(x_), y(y_), z(z_){}
    Vec3(const Vec3& vect){
        x = vect.x;
        y = vect.y;
        z = vect.z;
    }

    template<typename T2>
    explicit operator Vec3<T2>() const{
        return Vec3<T2>(T2(x), T2(y), T2(z));
    }

    Vec3 normalize(){
        const T length = len();
        return Vec3{x/length, y/length, z/length};
    }

    T len(){
        return std::sqrt(x * x + y * y + z * z);
    }

    Vec3& operator =(const Vec3& v){
        if (this == &v)
            return *this;
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    Vec3 operator +(const Vec3& vec) const{
        return Vec3(this->x + vec.x, this->y + vec.y, this->z + vec.z);
    }

    Vec3& operator +=(const Vec3& v){
        *this = *this + v;
        return *this;
    }

    Vec3 operator-() const{
        return Vec3(-x, -y, -z);
    }

    Vec3 operator-(const Vec3& v) const{
        return Vec3(this->x - v.x, this->y - v.y, this->z - v.z);
    }

    Vec3& operator -=(const Vec3& v){
        *this = *this - v;
        return *this;
    }

    Vec3 operator *(const T& val) const{
        return Vec3(x * val, y * val, z * val);
    }

    Vec3& operator*=(const T& val){
        *this = *this * val;
        return *this;
    }

    Vec3 operator /(const T& val) const{
        return Vec3(x / val, y / val, z / val);
    }

    Vec3& operator /=(const T& val){
        *this = *this / val;
        return *this;
    }

    T operator*(const Vec3& vec){
        return x * vec.x + y * vec.y + z * vec.z;
    }

    bool operator==(const Vec3& vec) const{
        return this->x == vec.x && this->y == vec.y && this->z == vec.z;
    }

    bool operator!=(const Vec3& vec) const{
        return !( *this == vec);
    }

    static Vec3 cross(const Vec3& a, const Vec3& b){
        return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }

    static T dot(const Vec3&a, const Vec3& b){
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    Vec3 refract(Vec3 direction, Vec3 normal, float eta){
        float n_dot_i = Vec3::dot(normal, direction);
        float k = 1.f - eta * eta * (1.f - n_dot_i * n_dot_i);
        if (k < 0.f)
            return Vec3{0.f, 0.f, 0.f};
        return eta * direction - (eta * n_dot_i + sqrt(k) * normal);
    }

    Vec3 reflect(Vec3 direction, Vec3 normal){
        return direction - 2.f * Vec3::dot(direction, normal) * normal;
    }

    public:
    T x, y, z;
};


using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
using Vec3i = Vec3<int>;

#endif // VEC3_H
