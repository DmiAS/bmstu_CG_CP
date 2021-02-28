#include "raythread.h"

Vec3f RayThread::toWorld(int x, int y){
    float ndc_x = (float)(x << 1) / (float)width - 1.f;
    float ndc_y = 1.f - (float)(y << 1) / (float)height;
    Vec4f res(ndc_x, ndc_y, 1);
    return res * inverse;
}

Vec3f RayThread::toWorld(const Vec3f &u, const Vec3f &v, const Vec3f &w, int x, int y){
    return u * float(x) - v * float(y) + w;
}

void RayThread::run()
{
    auto u = Vec3f::cross(cam->up, cam->direction).normalize();
    auto v = cam->up.normalize();
    auto w = -cam->direction.normalize();

    auto w_ = u * float(-(width >> 1)) + v * float(height >> 1) - w * (float((height >> 1)) / tan(cam->fov / 2 * M_PI / 180));

    for (int x = bound.xs; x <= bound.xe; x++){
        for (int y = bound.ys; y <= bound.ye; y++){
//            float px = (2 * ((float)x / (float)width) - 1) * tan(cam->fov / 2 * M_PI / 180) * cam->aspect_ratio;
//            float py = (1 - 2 * ((float)y / (float)height)) * tan(cam->fov / 2 * M_PI / 180);
//            auto d = Vec3f{px, py, 1}.normalize();
//            qDebug() << "direction 1 = " << d.x << d.y << d.z;
//            auto d = canvasToViewPort(x, y);
//            qDebug() << height / 2 << d.x << d.y << d.z;
//            return;
//            auto d = toWorld(x, y).normalize();
//            qDebug() << "direction 2 = " << m.x << m.y << m.z;
//            auto viewPlanePoint = viewPlaneBottomLeftPoint + xIncVector * (float)x + yIncVector * (float)y;
//            auto castRay = (viewPlanePoint - cam->position).normalize();
//            qDebug() << "new_direction = " << castRay.x << castRay.y << castRay.z;
//            autod = camera_d
//            auto color = traceRay(origin, d, 1, std::numeric_limits<float>::max(), 1) * 255.f;
            auto d = toWorld(u, v, w_, x, y).normalize();
            auto color = cast_ray(Ray(cam->position, d)) * 255.f;
            img.setPixelColor(x, y, QColor(color.x, color.y, color.z));
        }
    }

    emit finished();
}
