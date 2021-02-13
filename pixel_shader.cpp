#include "color_shader.h"
#include "bary.h"

Vec3f ColorShader::shade(const Vertex &a, const Vertex &b, const Vertex &c, const Vec3f& bary){
    auto pixel_color = baryCentricInterpolation(a.color, b.color, c.color, bary);
    #define TO_BYTE(v) (uint8_t) Clamp(255.f * GammaCorrect(v) + 0.5f, 0.f, 255.f)
    pixel_color.x = (TO_BYTE(pixel_color.x));
    pixel_color.y = (TO_BYTE(pixel_color.y));
    pixel_color.z = (TO_BYTE(pixel_color.z));
    #undef TO_BYTE
    return pixel_color;
}
