#include "texture.h"
#include "bary.h"
#include <QtDebug>

TextureShader::TextureShader(const std::string& fileName){
    texture.load(QString::fromStdString(fileName));
}

Vec3f TextureShader::shade(const Vertex &a, const Vertex &b, const Vertex &c, const Vec3f &bary){
    float pixel_z = interPolateCord(a.invW, b.invW, c.invW, bary);
    float invZ = 1 / pixel_z;

    auto face_color = baryCentricInterpolation(a.color, b.color, c.color, bary);

    float pixel_u = interPolateCord(a.u , b.u, c.u, bary) * invZ;
    float pixel_v = interPolateCord(a.v, b.v, c.v, bary) * invZ;

    int x = std::floor(pixel_u * (texture.width() - 1) );
    int y = std::floor(pixel_v * (texture.height() - 1));

//    auto color = texture.pixelColor(x, y);
//    Vec3f pixel_color = Vec3f{float(color.red() % 256), float(color.green() % 256), float(color.blue() % 256)};

//    auto t = pixel_color.normalize().hadamard(face_color).saturate() * 255.f;
    auto t = face_color.saturate() * 255.f;
//    pixel_color = pixel_color.hadamard(face_color).saturate() * 255.f;

    return t;
}
