#ifndef RAYTHREAD_H
#define RAYTHREAD_H
#include <QThread>
#include <QImage>
#include "model.h"
#include "light.h"

struct RayBound{
    int xs, xe;
    int ys, ye;
};

struct InterSectionData{
    Model model;
    float t;
    Vec3f normal;
};

class RayThread: public QThread
{
    Q_OBJECT
public:
    RayThread(QImage& img_, std::vector<Model*>& models_,
              Mat4x4f& inverse_, RayBound& bound_, int width_, int height_, Vec3f& origin_): img{img_},
        models{models_}, inverse{inverse_}, bound{bound_}, width{width_}, height{height_},
        origin{origin_}{}
protected:
    void run() override;
private:
    Vec3f toWorld(int x, int y);
    Vec3f traceRay(const Vec3f& o, const Vec3f& d, float t_min, float t_max, int depth);

    InterSectionData closestIntersection(const Vec3f& o, const Vec3f& d, float t_min, float t_max);

    Vec3f computeLightning(const Vec3f& p, const Vec3f& n, const Vec3f& direction, float specular);

private:
    QImage &img;
    std::vector<Model*>& models;
    Mat4x4f inverse;
    RayBound bound;
    int width, height;
    Vec3f origin;
};

#endif // RAYTHREAD_H
