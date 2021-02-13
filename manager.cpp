#include "scene_manager.h"
#include "bary.h"
#include "texture.h"
#include "geometry_shader.h"

#define NDCX_TO_RASTER(ndc_x, width) (((ndc_x + 1.0f) * (width >> 1)))
#define NDCY_TO_RASTER(ndc_y, height) (((1.0f - ndc_y) * (height >> 1)))
void denormolize(int width, int height, Vertex& v){
    v.pos.x = v.pos.x * v.invW;
    v.pos.y = v.pos.y * v.invW;
//    v.pos.z = v.pos.z * v.invW;
    v.pos.x = NDCX_TO_RASTER(v.pos.x, width);
    v.pos.y = NDCY_TO_RASTER(v.pos.y, height);
//    return Vec3f(point.x, point.y, point.z);
}


void SceneManager::init(){
    models.push_back(Model("C:\\raster\\ui_mode\\cube.obj"));
    qDebug() << models[0].vertex_buffer.size();
    pixel_shader = std::make_shared<TextureShader>("C:\\raster\\ui_mode\\bricks.jpg");
//    pixel_shader = std::make_shared<ColorShader>();
    vertex_shader = std::make_shared<VertexShader>();
    geom_shader = std::make_shared<GeometryShader>();
    render_all();
}

void SceneManager::render_all(){

    img.fill(Qt::black);

    for (auto& vec: depthBuffer)
        std::fill(vec.begin(), vec.end(), std::numeric_limits<float>::max());

    for (auto& model: models)
        rasterize(model);
    show();
}

bool SceneManager::backfaceCulling(const Vertex &a, const Vertex &b, const Vertex &c){
    auto cam = camers[curr_camera];

    auto normal = Vec3f::cross(b.pos - a.pos, c.pos - b.pos).normalize();
    auto res1 = Vec3f::dot(normal, b.pos - cam.position);
    auto res2 = Vec3f::dot(normal, c.pos - cam.position);
    auto res3 = Vec3f::dot(normal, a.pos - cam.position);

    if (res1 > 0 && res2 > 0 && res3 > 0)
        return true;
//    if ( (res1 > 0 || fabs(res1) < eps) && (res2 > 0 || fabs(res2) < eps) && (res3 > 0 || fabs(res3) < eps))
//        return true;
    return false;
}

bool SceneManager::clip(const Vertex& v){
    const float w = 1 / v.invW;
    return (v.pos.x > -w || fabs(v.pos.x + w) < eps) &&
           (v.pos.x < w || fabs(v.pos.x - w) < eps) &&
           (v.pos.y > -w || fabs(v.pos.y + w) < eps) &&
           (v.pos.y < w || fabs(v.pos.y - w) < eps) &&
           (v.pos.z > 0.f || fabs(v.pos.z) < eps) &&
           (v.pos.z < w || fabs(v.pos.z - w) < eps);
}

void SceneManager::rasterize(Model& model){
    auto cam = camers[curr_camera];
    auto objToWorld = model.objToWorld();
    auto viewMatrix = cam.viewMatrix();
    auto projMatrix = cam.projectionMatrix;

    for (int i = 0; i < model.index_buffer.size() / 3; i++){
        auto a = vertex_shader->shade(model.vertex_buffer[model.index_buffer[3 * i]], objToWorld, cam);
        auto b = vertex_shader->shade(model.vertex_buffer[model.index_buffer[3 * i + 1]], objToWorld, cam);
        auto c = vertex_shader->shade(model.vertex_buffer[model.index_buffer[3 * i + 2]], objToWorld, cam);

        if (backfaceCulling(a, b, c))
            continue;

        a = geom_shader->shade(a, projMatrix);
        b = geom_shader->shade(b, projMatrix);
        c = geom_shader->shade(c, projMatrix);

        rasterBarTriangle(a, b, c);
    }
}

#define Min(val1, val2) std::min(val1, val2)
#define Max(val1, val2) std::max(val1, val2)
void SceneManager::rasterBarTriangle(Vertex p1_, Vertex p2_, Vertex p3_){


    if (!clip(p1_) && !clip(p2_) && !clip(p3_)){
//        qDebug() << p1_.pos.x << p1_.pos.y << p1_.pos.z;
//        qDebug() << p2_.pos.x << p2_.pos.y << p2_.pos.z;
//        qDebug() << p3_.pos.x << p3_.pos.y << p3_.pos.z;
        return;
    }

    denormolize(width, height, p1_);
    denormolize(width, height, p2_);
    denormolize(width, height, p3_);

    auto p1 = p1_.pos;
    auto p2 = p2_.pos;
    auto p3 = p3_.pos;

    float sx = std::floor(Min(Min(p1.x, p2.x), p3.x));
    float ex = std::ceil(Max(Max(p1.x, p2.x), p3.x));

    float sy = std::floor(Min(Min(p1.y, p2.y), p3.y));
    float ey = std::ceil(Max(Max(p1.y, p2.y), p3.y));

    for (int y = static_cast<int>(sy); y < static_cast<int>(ey); y++){
        for (int x = static_cast<int>(sx); x < static_cast<int>(ex); x++){
            Vec3f bary = toBarycentric(p1, p2, p3, Vec3f(static_cast<float>(x), static_cast<float>(y)));
            if ( (bary.x > 0.0f || fabs(bary.x) < eps) && (bary.x < 1.0f || fabs(bary.x - 1.0f) < eps) &&
                 (bary.y > 0.0f || fabs(bary.y) < eps) && (bary.y < 1.0f || fabs(bary.y - 1.0f) < eps) &&
                 (bary.z > 0.0f || fabs(bary.z) < eps) && (bary.z < 1.0f || fabs(bary.z - 1.0f) < eps)){
                auto interpolated = baryCentricInterpolation(p1, p2, p3, bary);
                interpolated.x = x;
                interpolated.y = y;
                if (testAndSet(interpolated)){
                    auto pixel_color = pixel_shader->shade(p1_, p2_, p3_, bary);
//                    qDebug() << pixel_color.x, pixel_color.y, pixel_color.z;
                    img.setPixelColor(x, y, qRgb(pixel_color.x, pixel_color.y, pixel_color.z));
                }
            }
        }
    }

}

bool SceneManager::testAndSet(const Vec3f& p){
    int x = std::round(p.x), y = std::round(p.y);
    if (x >= width || y >= height || x < 0 || y < 0)
        return false;
    if (p.z < depthBuffer[x][y] || fabs(p.z - depthBuffer[x][y]) < eps){
        depthBuffer[x][y] = p.z;
        return true;
    }
    return false;
}

void SceneManager::show(){
    scene->addPixmap(QPixmap::fromImage(img));
}

void SceneManager::shift(trans_type t, float val){
    switch (t) {
    case shift_x:
        models[0].shiftX(val);
        break;
    case shift_y:
        models[0].shiftY(val);
        break;
    case shift_z:
        models[0].shiftZ(val);
    }

    render_all();
}

void SceneManager::rotate(trans_type t, float angle){
    switch (t) {
    case rot_x:
        models[0].rotateX(angle);
        break;
    case rot_y:
        models[0].rotateY(angle);
        break;
    case rot_z:
        models[0].rotateZ(angle);
    }

    render_all();
}

void SceneManager::scale(trans_type t, float factor){
    switch (t) {
    case scale_x:
        models[0].scaleX(factor);
        break;
    case scale_y:
        models[0].scaleY(factor);
        break;
    case scale_z:
        models[0].scaleZ(factor);
    }

    render_all();
}
