#ifndef SCENE_MANAGER_FIRST_H
#define SCENE_MANAGER_FIRST_H
#include <vector>
#include <numeric>
#include <QImage>
#include <QGraphicsScene>
#include "model.h"
#include "camera.h"
#include "color_shader.h"
#include "vertex_shader.h"
#include <QtDebug>

enum trans_type{shift_x, shift_y, shift_z, rot_x, rot_y, rot_z, scale_x, scale_y, scale_z};

class SceneManager{

public:

    SceneManager() = default;

    SceneManager(int width_, int height_, QColor background_color_, QGraphicsScene* scene_): width{width_}, height{height_},
        background_color(background_color_), scene{scene_}{

        img = QImage(width, height, QImage::Format_RGB32);
        depthBuffer.resize(width);
        for (auto &vec : depthBuffer)
            vec.resize(height, std::numeric_limits<float>::max());
        img.fill(background_color);

        camers.push_back(Camera(width, height));
    }

    void init();

    void shift(trans_type t, float val);

    void rotate(trans_type t, float angle);

    void scale(trans_type t, float factor);

private:
    void render_all();

    void rasterize(Model& model);

    void show();

    void rasterBarTriangle(Vertex p1_, Vertex p2_, Vertex p3_);

    bool testAndSet(Vec3f p);

    bool backfaceCulling(const Vertex& a, const Vertex& b, const Vertex& c);


private:
    std::vector<Camera> camers;
    int curr_camera = 0;
    std::vector<Model> models;
    int width, height;
    std::vector<std::vector<float>> depthBuffer;
    QImage img;
    QColor background_color;
    QGraphicsScene *scene;
    std::shared_ptr<PixelShaderInterface> pixel_shader;
    std::shared_ptr<VertexShaderInterface> vertex_shader;
    std::shared_ptr<GeometryShaderInterface> geom_shader;
};
#endif // SCENE_MANAGER_H
