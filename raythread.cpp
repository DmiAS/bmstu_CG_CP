#include "raythread.h"

Vec3f RayThread::toWorld(int x, int y){
    float ndc_x = (float)(x << 1) / (float)width - 1.f;
    float ndc_y = 1.f - (float)(y << 1) / (float)height;
    Vec4f res(ndc_x, ndc_y, 1);
    return res * inverse;
}

void RayThread::run()
{
    for (int x = bound.xs; x <= bound.xe; x++){
        for (int y = bound.ys; y <= bound.ye; y++){
//            float px = (2 * ((float)x / (float)width) - 1) * tan(cam.fov / 2 * M_PI / 180) * cam.aspect_ratio;
//            float py = (1 - 2 * ((float)y / (float)height)) * tan(cam.fov / 2 * M_PI / 180);
//            auto d = Vec3f{px, py, 1}.normalize();
//            auto d = canvasToViewPort(x, y);
//            qDebug() << height / 2 << d.x << d.y << d.z;
//            return;
            auto d = toWorld(x, y).normalize();
            auto color = traceRay(origin, d, 1, std::numeric_limits<float>::max(), 1) * 255.f;
            img.setPixelColor(x, y, QColor(color.x, color.y, color.z));
//            img.setPixelColor(x, y, Qt::red);
        }
    }
}
