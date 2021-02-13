#include "scene_manager.h"
#include "bary.h"
#include "texture.h"

#define NDCX_TO_RASTER(ndc_x, width) (((ndc_x + 1.0f) * (width >> 1)))
#define NDCY_TO_RASTER(ndc_y, height) (((1.0f - ndc_y) * (height >> 1)))
Vec3f denormolize(int width, int height, Vec3f point){
    point.x = NDCX_TO_RASTER(point.x, width);
    point.y = NDCY_TO_RASTER(point.y, height);
    return Vec3f(point.x, point.y, point.z);
}


void SceneManager::init(){
    models.push_back(Model("C:\\raster\\ui_mode\\cube.obj"));
    qDebug() << models[0].vertex_buffer.size();
//    pixel_shader = std::make_shared<TextureShader>("C:\\raster\\ui_mode\\wall.jpg");
    pixel_shader = std::make_shared<ColorShader>();
    vertex_shader = std::make_shared<VertexShader>();
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

void SceneManager::rasterize(Model& model){
    auto cam = camers[curr_camera];
    auto projMatrix = cam.cameraProjectMatrix();
    for (int i = 0; i < model.index_buffer.size() / 3; i++){
        auto a = vertex_shader->shade(model.vertex_buffer[model.index_buffer[3 * i]], model.objToWorld(), projMatrix);
        auto b = vertex_shader->shade(model.vertex_buffer[model.index_buffer[3 * i + 1]], model.objToWorld(), projMatrix);
        auto c = vertex_shader->shade(model.vertex_buffer[model.index_buffer[3 * i + 2]], model.objToWorld(), projMatrix);


        rasterBarTriangle(a, b, c);
    }
}

#define Min(val1, val2) std::min(val1, val2)
#define Max(val1, val2) std::max(val1, val2)
void SceneManager::rasterBarTriangle(Vertex p1_, Vertex p2_, Vertex p3_){
    Vec3f p1 = denormolize(width, height, p1_.pos);
    Vec3f p2 = denormolize(width, height, p2_.pos);
    Vec3f p3 = denormolize(width, height, p3_.pos);

    float sx = std::floor(Min(Min(p1.x, p2.x), p3.x));
    float ex = std::ceil(Max(Max(p1.x, p2.x), p3.x));

    float sy = std::floor(Min(Min(p1.y, p2.y), p3.y));
    float ey = std::ceil(Max(Max(p1.y, p2.y), p3.y));

    p1_.pos = p1;
    p2_.pos = p2;
    p3_.pos = p3;

    for (int y = static_cast<int>(sy); y < static_cast<int>(ey); y++){
        for (int x = static_cast<int>(sx); x < static_cast<int>(ex); x++){
            Vec3f bary = toBarycentric(p1, p2, p3, Vec3f(static_cast<float>(x), static_cast<float>(y)));
            if ( (bary.x > 0.0f || fabs(bary.x) < eps) && (bary.x < 1.0f || fabs(bary.x - 1.0f) < eps) &&
                 (bary.y > 0.0f || fabs(bary.y) < eps) && (bary.y < 1.0f || fabs(bary.y - 1.0f) < eps) &&
                 (bary.z > 0.0f || fabs(bary.z) < eps) && (bary.z < 1.0f || fabs(bary.z - 1.0f) < eps)){
                auto interpolated = baryCentricInterpolation(p1, p2, p3, bary);
                if (testAndSet(interpolated)){
                    auto pixel_color = pixel_shader->shade(p1_, p2_, p3_, bary);
//                    qDebug() << pixel_color.x, pixel_color.y, pixel_color.z;
                    img.setPixelColor(x, y, qRgb(pixel_color.x, pixel_color.y, pixel_color.z));
                }
            }
        }
    }

}

bool SceneManager::testAndSet(Vec3f p){
    int x = std::round(p.x), y = std::round(p.y);
    if (x >= width || y >= height || x < 0 || y < 0)
        return false;
    if (p.z < depthBuffer[x][y]){
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
