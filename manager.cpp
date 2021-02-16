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
    pixel_shader = std::make_shared<TextureShader>("C:\\raster\\ui_mode\\bricks.jpg");
//    pixel_shader = std::make_shared<ColorShader>();
    vertex_shader = std::make_shared<VertexShader>();
    geom_shader = std::make_shared<GeometryShader>();
    render_all();
}

void SceneManager::render_all(){
//    qDebug() << "rendering";

    img.fill(Qt::black);

    for (auto& vec: depthBuffer)
        std::fill(vec.begin(), vec.end(), std::numeric_limits<float>::max());

    for (auto& model: models)
        rasterize(model);
    show();
}

bool SceneManager::backfaceCulling(const Vertex &a, const Vertex &b, const Vertex &c){
    auto cam = camers[curr_camera];

    auto face_normal = Vec3f::cross(b.pos - a.pos, c.pos - b.pos);
//    if (Vec3f::dot(face_normal, a.normal) < 0)
//        face_normal *= -1.f;

    auto res1 = Vec3f::dot(face_normal, a.pos - cam.position);
    auto res2 = Vec3f::dot(face_normal, b.pos - cam.position);
    auto res3 = Vec3f::dot(face_normal, c.pos - cam.position);

    if ((res1 > 0 || fabs(res1) < eps) && (res2 > 0 || fabs(res2) < eps) && (res3 > 0 || fabs(res3) < eps))
        return true;
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
    auto rotation_matrix = model.rotation_matrix;
    auto objToWorld = model.objToWorld();
    auto viewMatrix = cam.viewMatrix();
    auto projMatrix = cam.projectionMatrix;

    for (int i = 0; i < model.index_buffer.size() / 3; i++){
        auto a = vertex_shader->shade(model.vertex_buffer[model.index_buffer[3 * i]], rotation_matrix, objToWorld, cam);
        auto b = vertex_shader->shade(model.vertex_buffer[model.index_buffer[3 * i + 1]], rotation_matrix, objToWorld, cam);
        auto c = vertex_shader->shade(model.vertex_buffer[model.index_buffer[3 * i + 2]], rotation_matrix, objToWorld, cam);

//        if (backfaceCulling(a, b, c))
//            continue;

        a = geom_shader->shade(a, projMatrix);
        b = geom_shader->shade(b, projMatrix);
        c = geom_shader->shade(c, projMatrix);

        rasterBarTriangle(a, b, c);
    }
}

bool winding_order(const Vec3f& p1, const Vec3f& p2, const Vec3f& p3){
//    Vec3f edge0, edge1;
    return 0 > (p2.x - p1.x) * (p2.y - p1.y) + (p3.x - p2.x) * (p3.y - p2.y);
}

#define Min(val1, val2) std::min(val1, val2)
#define Max(val1, val2) std::max(val1, val2)
void SceneManager::rasterBarTriangle(Vertex p1_, Vertex p2_, Vertex p3_){

    if (!clip(p1_) && !clip(p2_) && !clip(p3_)){
        return;
    }

//    // winding order test
//    if (winding_order(p1_.pos, p2_.pos, p3_.pos)) return;

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
                    auto pixel_color = pixel_shader->shade(p1_, p2_, p3_, bary) * 255.f;
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
        models[current_model].shiftX(val);
        break;
    case shift_y:
        models[current_model].shiftY(val);
        break;
    case shift_z:
        models[current_model].shiftZ(val);
    }

    render_all();
}

void SceneManager::rotate(trans_type t, float angle){
    switch (t) {
    case rot_x:
        models[current_model].rotateX(angle);
        break;
    case rot_y:
        models[current_model].rotateY(angle);
        break;
    case rot_z:
        models[current_model].rotateZ(angle);
    }

    render_all();
}

void SceneManager::scale(trans_type t, float factor){
    switch (t) {
    case scale_x:
        models[current_model].scaleX(factor);
        break;
    case scale_y:
        models[current_model].scaleY(factor);
        break;
    case scale_z:
        models[current_model].scaleZ(factor);
    }

    render_all();
}

void SceneManager::moveCamera(trans_type t, float dist){
    auto& cam = camers[curr_camera];
    std::function<void ()> change_func;
    switch (t) {
    case shift_x:
        change_func = [&](){
            cam.shiftX(dist);
        };
        break;
    case shift_z:
        change_func = [&](){
            cam.shiftZ(dist);
        };
        break;
    case rot_x:
        change_func = [&](){
            cam.rotateX(dist);
        };
        break;
    case rot_y:
        change_func = [&](){
            cam.rotateY(dist);
        };
        break;
    default:
        return;
    }

    change_func();

    render_all();
}

void SceneManager::uploadModel(std::string name, uint32_t& uid){

    const std::map<std::string, std::string> files = {
        {"Куб", "C:\\raster\\ui_mode\\cube.obj"},
        {"Сфера", "C:\\raster\\ui_mode\\sphere.obj"},
        {"Пирамида", "C:\\raster\\ui_mode\\pyramyd.obj"},
        {"Конус", "C:\\raster\\ui_mode\\conus.obj"},
    };

    if (!files.count(name))
        return;

    uid = models_index++;
    models.push_back(Model(files.at(name), uid));

    render_all();
}


void SceneManager::removeModel(){
    models.erase(models.begin() + current_model);
    render_all();
}

void SceneManager::setCurrentModel(uint32_t uid){
    auto it = models.begin();
    int i = 0;
    for (; it <  models.end(); it++, i++)
        if (it->getUid() == uid)
            break;
    current_model = i;
}

void SceneManager::setColor(const Vec3f &color){
    models[current_model].setColor(color);
    render_all();
}
