#include "color_shader.h"
#include "bary.h"

Vec3f ColorShader::shade(const Vertex &a, const Vertex &b, const Vertex &c, const Vec3f& bary){
    auto pixel_color = baryCentricInterpolation(a.color, b.color, c.color, bary);
//    #define TO_BYTE(v) (uint8_t) Clamp(255.f * GammaCorrect(v) + 0.5f, 0.f, 255.f)
//    pixel_color.x = (TO_BYTE(pixel_color.x));
//    pixel_color.y = (TO_BYTE(pixel_color.y));
//    pixel_color.z = (TO_BYTE(pixel_color.z));
//    #undef TO_BYTE
//    Vec3f light_color = {1.0f, 1.0f, 1.0f};
//    Vec3f direction = {0.f, 0.f, 1.f};
//    Vec3f ambient = {0.3f, 0.3f, 0.3f};
//    auto normal = baryCentricInterpolation(a.normal, b.normal, c.normal, bary);

//    auto p = baryCentricInterpolation(a.pos, b.pos, c.pos, bary);

////    auto diffuse = light_color * std::max(0.f, Vec3f::dot(normal.normalize(), -(p - Vec3f{0.f, 0.f, -5.f}).normalize()));
////    auto c_ = (diffuse + ambient).saturate();
//    auto light_direction = (Vec3f{0.f, 0.f, -5.f} - p).normalize();

//    float diff = std::max(Vec3f::dot(normal, light_direction), 0.f);
//    pixel_color = pixel_color * diff + pixel_color * 0.4f;
//    return pixel_color.saturate();
    return pixel_color;
}
