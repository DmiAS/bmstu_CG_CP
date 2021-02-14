#ifndef MAT_H
#define MAT_H
#include "vec3.h"
#include "vec4.h"
#include <cmath>
#include <cstring>

template <typename T, size_t S>
class Matrix{
public:

    Matrix& operator =(const Matrix& m){
        memcpy(elements, m.elements, sizeof(elements));
        return *this;
    }

    Matrix operator *(const T& val) const{
        Matrix res = *this;
        for( auto& row : res.elements )
        {
            for( T& e : row )
            {
                e *= val;
            }
        }
        return res;
    }

    Matrix& operator *=(const T& val){
        *this = *this * val;
        return *this;
    }

    Matrix operator *(const Matrix& mat) const{
        Matrix result;
        for( size_t j = 0; j < S; j++ )
        {
            for( size_t k = 0; k < S; k++ )
            {
                T sum = (T)0.0;
                for( size_t i = 0; i < S; i++ )
                {
                    sum += elements[j][i] * mat.elements[i][k];
                }
                result.elements[j][k] = sum;
            }
        }
        return result;
    }

    Matrix& operator *=(const Matrix& mat){
        *this = *this * mat;
        return *this;
    }

    // транспонирование
    Matrix operator !() const{
        Matrix xp;
        for( size_t j = 0; j < S; j++ )
        {
            for( size_t k = 0; k < S; k++ )
            {
                xp.elements[j][k] = elements[k][j];
            }
        }
        return xp;
    }

    static Matrix Identity(){
        if constexpr (S == 3){
            return {
                (T)1.0,(T)0.0,(T)0.0,
                (T)0.0,(T)1.0,(T)0.0,
                (T)0.0,(T)0.0,(T)1.0,
            };
        } else if constexpr (S == 4){
            return {
                (T)1.0,(T)0.0,(T)0.0,(T)0.0,
                (T)0.0,(T)1.0,(T)0.0,(T)0.0,
                (T)0.0,(T)0.0,(T)1.0,(T)0.0,
                (T)0.0,(T)0.0,(T)0.0,(T)1.0,
            };
        }
    }

    static Matrix Scaling(const T& factor_x, const T& factor_y, const T& factor_z){
        if constexpr(S == 3){
            return{
                factor_x,(T)0.0,(T)0.0,
                (T)0.0,factor_y,(T)0.0,
                (T)0.0,(T)0.0,factor_z,
            };
        } else if constexpr( S == 4 ){
            return {
                factor_x,(T)0.0,(T)0.0,(T)0.0,
                (T)0.0,factor_y,(T)0.0,(T)0.0,
                (T)0.0,(T)0.0,factor_z,(T)0.0,
                (T)0.0,(T)0.0,(T)0.0,(T)1.0,
            };
        }
    }

    static Matrix ScaleX(const T& factor){
        if constexpr(S == 3){
            return{
                factor,(T)0.0,(T)0.0,
                (T)0.0,(T)0.0,(T)0.0,
                (T)0.0,(T)0.0,(T)0.0,
            };
        } else if constexpr( S == 4 ){
            return {
                factor,(T)0.0,(T)0.0,(T)0.0,
                (T)0.0,(T)0.0,(T)0.0,(T)0.0,
                (T)0.0,(T)0.0,(T)0.0,(T)0.0,
                (T)0.0,(T)0.0,(T)0.0,(T)1.0,
            };
        }
    }


    static Matrix ScaleY(const T& factor){
        if constexpr(S == 3){
            return{
                (T)0.0,(T)0.0,(T)0.0,
                (T)0.0,factor,(T)0.0,
                (T)0.0,(T)0.0,(T)0.0,
            };
        } else if constexpr( S == 4 ){
            return {
                (T)0.0,(T)0.0,(T)0.0,(T)0.0,
                (T)0.0,factor,(T)0.0,(T)0.0,
                (T)0.0,(T)0.0,(T)0.0,(T)0.0,
                (T)0.0,(T)0.0,(T)0.0,(T)1.0,
            };
        }
    }

    static Matrix ScaleZ(const T& factor){
        if constexpr(S == 3){
            return{
                (T)0.0,(T)0.0,(T)0.0,
                (T)0.0,(T)0.0,(T)0.0,
                (T)0.0,(T)0.0,factor,
            };
        } else if constexpr( S == 4 ){
            return {
                (T)0.0,(T)0.0,(T)0.0,(T)0.0,
                (T)0.0,(T)0.0,(T)0.0,(T)0.0,
                (T)0.0,(T)0.0,factor,(T)0.0,
                (T)0.0,(T)0.0,(T)0.0,(T)1.0,
            };
        }
    }

    static Matrix RotationZ(const T& theta){
        const T angle = theta * M_PI / (T)180;
        const T sinTheta = sin( angle );
        const T cosTheta = cos( angle );
        if constexpr( S == 3 )
        {
            return{
                 cosTheta, sinTheta, (T)0.0,
                -sinTheta, cosTheta, (T)0.0,
                (T)0.0,    (T)0.0,   (T)1.0,
            };
        }
        else if constexpr( S == 4 )
        {
            return {
                 cosTheta, sinTheta, (T)0.0,(T)0.0,
                -sinTheta, cosTheta, (T)0.0,(T)0.0,
                (T)0.0,    (T)0.0,   (T)1.0,(T)0.0,
                (T)0.0,	   (T)0.0,   (T)0.0,(T)1.0,
            };
        }
    }

    static Matrix RotationY(const T& theta){
        const T angle = theta * M_PI / (T)180;
        const T sinTheta = sin( angle );
        const T cosTheta = cos( angle );
        if constexpr( S == 3 )
        {
            return{
                 cosTheta, (T)0.0,-sinTheta,
                 (T)0.0,   (T)1.0, (T)0.0,
                 sinTheta, (T)0.0, cosTheta
            };
        }
        else if constexpr( S == 4 )
        {
            return {
                cosTheta, (T)0.0, -sinTheta,(T)0.0,
                (T)0.0,   (T)1.0, (T)0.0,   (T)0.0,
                sinTheta, (T)0.0, cosTheta, (T)0.0,
                (T)0.0,   (T)0.0, (T)0.0,   (T)1.0,
            };
        }
    }

    static Matrix RotationX(const T& theta){
        const T angle = theta * M_PI / (T)180;
        const T sinTheta = sin( angle );
        const T cosTheta = cos( angle );
        if constexpr( S == 3 )
        {
            return{
                (T)1.0, (T)0.0,   (T)0.0,
                (T)0.0, cosTheta, sinTheta,
                (T)0.0,-sinTheta, cosTheta,
            };
        }
        else if constexpr( S == 4 )
        {
            return {
                (T)1.0, (T)0.0,   (T)0.0,  (T)0.0,
                (T)0.0, cosTheta, sinTheta,(T)0.0,
                (T)0.0,-sinTheta, cosTheta,(T)0.0,
                (T)0.0, (T)0.0,   (T)0.0,  (T)1.0,
            };
        }
    }

    static Matrix Translation(T x, T y, T z){
        if constexpr( S == 4 )
        {
            return {
                (T)1.0,(T)0.0,(T)0.0,(T)0.0,
                (T)0.0,(T)1.0,(T)0.0,(T)0.0,
                (T)0.0,(T)0.0,(T)1.0,(T)0.0,
                x,     y,      z,    (T)1.0,
            };
        }
    }

    static Matrix ProjectionFOV(T fov,T ar,T n,T f){
        if constexpr( S == 4 ){
            const T fov_rad = fov * (T)M_PI / (T)180.0;
            const T yScale = (T)1.0 / (T)std::tan(fov_rad * (T)0.5);
            const T xScale = yScale / ar;
            return{
                xScale,  (T)0.0,  (T)0.0,				(T)0.0,
                (T)0.0,  yScale,	(T)0.0,				(T)0.0,
                (T)0.0,	 (T)0.0,	 (f / (f - n)),  (T)1.0,
                (T)0.0,	 (T)0.0,	 (-(f * n) / (f - n)),	(T)0.0,
            };
        }
    }

    static Matrix LookAtLH(Vec3f position, Vec3f at, Vec3f up){
        if constexpr (S == 4){
            auto zaxis = (at - position).normalize();
            auto xaxis = (Vec3f::cross(up, zaxis)).normalize();
            auto yaxis = Vec3f::cross(zaxis, xaxis);
            return {
                xaxis.x,  yaxis.x, zaxis.x, 0,
                xaxis.y,  yaxis.y, zaxis.y, 0,
                xaxis.z,  yaxis.z, zaxis.z, 0,
                -Vec3f::dot(xaxis, position), -Vec3f::dot(yaxis, position), -Vec3f::dot(zaxis, position) , 1
            };
        }
    }

    static Matrix RotationMatrix(Vec3f p, float angle){
        if constexpr (S == 4){
            angle = angle * (T)M_PI / (T)180.0;
            float sin = std::sin(angle), cos = std::cos(angle);
            return {
            cos+p.x*p.x*(1-cos), p.x*p.y*(1-cos)-p.z*sin, p.x*p.z*(1-cos)+p.y*sin, 0,
            p.y*p.x*(1-cos)+p.z*sin,  cos+p.y*p.y*(1-cos), p.y*p.z*(1-cos)-p.x*sin, 0,
            p.z*p.x*(1-cos)-p.y*sin, p.z*p.y*(1-cos)+p.x*sin, cos+p.z*p.z*(1-cos),  0,
                    0, 0, 0, 1
        };
        }
    }

public:
    T elements[S][S];
};

//template <typename T,size_t S>
//class _Mat
//{
//public:

//    constexpr static _Mat Projection( T w,T h,T n,T f )
//    {
//        if constexpr( S == 4 )
//        {
//            return {
//                (T)2.0 * n / w,	(T)0.0,			(T)0.0,				(T)0.0,
//                (T)0.0,			(T)2.0 * n / h,	(T)0.0,				(T)0.0,
//                (T)0.0,			(T)0.0,			f / (f - n),		(T)1.0,
//                (T)0.0,			(T)0.0,			-n * f / (f - n),	(T)0.0,
//            };
//        }
//    }

//public:
//    T elements[S][S];
//};

template<typename T>
Vec3<T>& operator *=(Vec3<T>& l, const Matrix<T, 3>& r){
    return l = l * r;
}

template<typename T>
Vec3<T> operator *(const Vec3<T>& lhs, const Matrix<T,3>& rhs){
    return{
        lhs.x * rhs.elements[0][0] + lhs.y * rhs.elements[1][0] + lhs.z * rhs.elements[2][0],
        lhs.x * rhs.elements[0][1] + lhs.y * rhs.elements[1][1] + lhs.z * rhs.elements[2][1],
        lhs.x * rhs.elements[0][2] + lhs.y * rhs.elements[1][2] + lhs.z * rhs.elements[2][2]
    };
}

template <typename T>
Vec4<T>& operator *=(const Vec4<T>& lhs, const Matrix<T, 4>& rhs){
    return lhs = lhs * rhs;
}

template <typename T>
Vec4<T> operator *(const Vec4<T>& lhs, const Matrix<T, 4>& rhs){
    return{
        lhs.x * rhs.elements[0][0] + lhs.y * rhs.elements[1][0] + lhs.z * rhs.elements[2][0] + lhs.w * rhs.elements[3][0],
        lhs.x * rhs.elements[0][1] + lhs.y * rhs.elements[1][1] + lhs.z * rhs.elements[2][1] + lhs.w * rhs.elements[3][1],
        lhs.x * rhs.elements[0][2] + lhs.y * rhs.elements[1][2] + lhs.z * rhs.elements[2][2] + lhs.w * rhs.elements[3][2],
        lhs.x * rhs.elements[0][3] + lhs.y * rhs.elements[1][3] + lhs.z * rhs.elements[2][3] + lhs.w * rhs.elements[3][3]
    };
}

using Mat3x3f = Matrix<float, 3>;
using Mat3x3d = Matrix<double, 3>;
using Mat4x4f = Matrix<float, 4>;
using Mat4x4d = Matrix<double, 4>;
#endif // MAT_H
