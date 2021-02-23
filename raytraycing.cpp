#include "raythread.h"
#include "scene_manager.h"

const float eps_float = 1e-5;
bool checkIntersection(const float& t, const float& t_min, const float& t_max, const float& closest_t){
    return t > t_min && t < t_max && t < closest_t;
}


Vec3f reflect(const Vec3f &L, const Vec3f &N) {
//    return I - Vec3f::dot(N*2.f, Vec3f::dot(I, N));
    return L - (N * 2.f * Vec3f::dot(N, L));
}

Vec3f refract(const Vec3f &I, const Vec3f &N, const float eta_t, const float eta_i=1.f) { // Snell's law
    float cosi = - std::max(-1.f, std::min(1.f, Vec3f::dot(I, N)));
    if (cosi<0) return refract(I, -N, eta_i, eta_t); // if the ray comes from the inside the object, swap the air and the media
    float eta = eta_i / eta_t;
    float k = 1 - eta*eta*(1 - cosi*cosi);
    return k<0 ? Vec3f(1,0,0) : I*eta + N*(eta*cosi - sqrtf(k)); // k<0 = total reflection, no ray to refract. I refract it anyways, this has no physical meaning
}

//Vec3f RayThread::computeLightning(const Vec3f &p, const Vec3f &n, const Vec3f &direction, float specular, int depth){
//    Vec3f i;
//    Vec3f lightDir;
//    specular = 0.6f;
//    float di = 0.3f;

//    float distance = 0.f;

//    Vec3f ambient, diffuse = {0.f, 0.f, 0.f}, spec = {0.f, 0.f, 0.f};

////    Vec3f reflect_dir = reflect(-direction, n).normalize();

////    Vec3f reflect_orig = Vec3f::dot(reflect_dir, n) < 0 ? p - n * 1e-3 : p + n * 1e-3;
////    Vec3f reflect_color = cast_ray(Ray(reflect_orig, reflect_dir), depth + 1);

////    Vec3f refract_dir = refract(direction, n, 10).normalize();
////    Vec3f refract_orig = Vec3f::dot(refract_dir, n) < 0 ? p - n * 1e-3 : p + n * 1e3;
////    Vec3f refract_color = cast_ray(Ray(refract_orig, refract_dir), depth + 1);

//    for (auto &model: models){
//        if (model->isObject()) continue;
//        Light* light = dynamic_cast<Light*>(model);
//        if (light->t == Light::light_type::ambient)
////            i += light->color_intensity;
//            ambient = light->color_intensity;
//        else{
//            float t_max;
//            if (light->t == Light::light_type::point){
////                L = Vec3f{light->position.x - p.x,
////                     light->position.y - p.y,
////                     light->position.z - p.z,
////                    }.normalize();
////                t_max = 1;
//                lightDir = (light->position - p);
//                distance = lightDir.len();
//                lightDir = lightDir.normalize();
////                L = (p - light->position).normalize();
//            } else{
//                lightDir = light->direction;
//                t_max = std::numeric_limits<float>::max();
//            }

//            auto tDot = Vec3f::dot(lightDir, n);

//            Vec3f shadow_orig = tDot < 0 ? p - n*1e-3 : p + n*1e-3; // checking if the point lies in the shadow of the lights[i]
//            InterSectionData tmpData;
//            if (sceneIntersect(Ray(shadow_orig, lightDir), tmpData))
//                if ((tmpData.point - shadow_orig).len() < distance)
//                    continue;

//            diffuse = (light->color_intensity * std::max(0.f, Vec3f::dot(n, lightDir)) * di);
//            auto r = reflect(lightDir, n);
//            auto r_dot = Vec3f::dot(r, -direction);
//            auto power = powf(std::max(0.f, r_dot), 20);
//            spec = light->color_intensity * power * specular;
//        }

//    }

//    return (ambient + diffuse + spec + refract_color).saturate();
//}

//Vec3f reflectRay(const Vec3f& r, const Vec3f& n){
//    return (n * 2 * Vec3f::dot(n, r)) - r;
//}

//Vec3f RayThread::traceRay(const Vec3f& o, const Vec3f& d, float t_min, float t_max, int depth){
//    auto res = closestIntersection(o, d, t_min, t_max); // Model - closest_t

//    if (fabs(res.t -  std::numeric_limits<float>::max()) < eps_float)
//        return Vec3f{0, 0, 0};
////    return res.model.color * 255.f;

//    auto p = o + d * res.t;

//    auto computed_light = computeLightning(p, res.normal, -d, res.model.specular);

//    auto local_color = res.model.color.hadamard(computed_light);

////    float r = res.model.reflective;
////    if (depth <= 0 || r <= 0){
////        return local_color;
////    }

////    if (r >= 0){
////        auto R = reflectRay(-d, res.normal);
////        auto reflected_color = traceRay(p, R, 1e-3, std::numeric_limits<float>::max(),
////                                        depth - 1);
////        local_color = local_color * (1 - r) + reflected_color * r;
////    }

//    return local_color;

////    Vec3f p = o + d * res.second;

//}

bool RayThread::sceneIntersect(const Ray &ray, InterSectionData &data, float t_max){
    float closeset_t = std::numeric_limits<float>::max();
    bool intersected = false;
    InterSectionData d;
    int cnt = 0;
    for (auto& model: models){
        if (!model->isObject()) continue;
        if (model->intersect(ray, d) && d.t < closeset_t){
            closeset_t = d.t;
            intersected = true;
            data = d;
//            data.point = ray.origin + ray.direction * data.t;
            data.model = *model;
        }
    }

    return intersected;
}


Vec3f RayThread::cast_ray(const Ray &ray, int depth){
    float t_min = 1e-3, t_max = std::numeric_limits<float>::max();
    InterSectionData data;
    if (depth > 2 || !sceneIntersect(ray, data))
        return Vec3f{0.f, 0, 0};

//    if (depth > 0){
//        qDebug() << "intersected" << ray.direction.x << ray.direction.y << ray.direction.z;
//        qDebug() << "point = " << data.point.x << data.point.y << data.point.z;
//        qDebug() << "t = " << data.t;
//        qDebug() << "origin = " << ray.origin.x << ray.origin.y << ray.origin.z;
//    }

    float specular = 0.6f;
    float di = 0.3f;

    float distance = 0.f;

    float occlusion = 1e-4f;
    Vec3f ambient, diffuse = {0.f, 0.f, 0.f}, spec = {0.f, 0.f, 0.f}, lightDir = {0.f, 0.f, 0.f}, reflect_color = {0.f, 0.f, 0.f};

    Vec3f refract_dir = refract(ray.direction, data.normal, 125.f).normalize();
    Vec3f refract_orig = Vec3f::dot(refract_dir, data.normal) < 0 ? data.point - data.normal * 1e-3f : data.point + data.normal * 1e3f;
    Vec3f refract_color = cast_ray(Ray(refract_orig, refract_dir), depth + 1);

//    Vec3f reflect_dir = reflect(ray.direction, data.normal).normalize();
//    Vec3f reflect_orig = Vec3f::dot(reflect_dir, data.normal) < 0 ? data.point - data.normal * 1e-3f : data.point + data.normal * 1e-3f;
//    reflect_color = cast_ray(Ray(reflect_orig, reflect_dir), depth + 1);


    for (auto &model: models){
        if (model->isObject()) continue;
        Light* light = dynamic_cast<Light*>(model);
        if (light->t == Light::light_type::ambient)
//            i += light->color_intensity;
            ambient = light->color_intensity;
        else{
            if (light->t == Light::light_type::point){
//                L = Vec3f{light->position.x - p.x,
//                     light->position.y - p.y,
//                     light->position.z - p.z,
//                    }.normalize();
//                t_max = 1;
                lightDir = (light->position - data.point);
                distance = lightDir.len();
                lightDir = lightDir.normalize();
//                L = (p - light->position).normalize();
            } else{
                lightDir = light->direction;
            }

            auto tDot = Vec3f::dot(lightDir, data.normal);

            Vec3f shadow_orig = tDot < 0 ? data.point - data.normal*occlusion : data.point + data.normal*occlusion; // checking if the point lies in the shadow of the lights[i]
            InterSectionData tmpData;
            if (sceneIntersect(Ray(shadow_orig, lightDir), tmpData))
                if ((tmpData.point - shadow_orig).len() < distance)
                    continue;

//            Vec3f reflect_dir = reflect(ray.direction, data.normal).normalize();
//            Vec3f reflect_orig = Vec3f::dot(reflect_dir, data.normal) < 0 ? data.point - data.normal * 1e-3f : data.point + data.normal * 1e-3f;
//            reflect_color = cast_ray(Ray(reflect_orig, reflect_dir), depth + 1);

            diffuse = (light->color_intensity * std::max(0.f, Vec3f::dot(data.normal, lightDir)) * di);
            auto r = reflect(lightDir, data.normal);
            auto r_dot = Vec3f::dot(r, ray.direction);
            auto power = powf(std::max(0.f, r_dot), 20);
            spec = light->color_intensity * power * specular;
        }

    }

//    auto local_color = data.color.hadamard(computeLightning(data.point, data.normal, -ray.direction, data.model.specular)).saturate();
    auto local_color = data.color.hadamard(ambient + diffuse + spec + reflect_color * 0.0f + refract_color).saturate();
//    auto local_color = data.color.hadamard(ambient).saturate();
    return local_color;
}

Vec4f toWorld(int x, int y, const Mat4x4f& inverse, int width, int height){
    float ndc_x = (float)(x << 1) / (float)width - 1.f;
    float ndc_y = 1.f - (float)(y << 1) / (float)height;
    Vec4f res(ndc_x, ndc_y, 1);
    return res * inverse;
}

std::vector<RayBound> subBlock(const RayBound& bound, int depth){
    if (depth == 0){
        return {RayBound{.xs = bound.xs, .xe = bound.xe, .ys = bound.ys, .ye = bound.ye}};
    }
    std::vector<RayBound> output;
    int x_q = (bound.xe - bound.xs) % 2 == 0;
    int y_q = (bound.ye - bound.ys) % 2 == 0;
    int x_step = bound.xe / 2;
    int y_step = bound.ye / 2;
    for (int curr = x_step - x_q, prev = bound.xs; curr <= bound.xe;){
        for (int curr_y = y_step - y_q, prev_y = bound.ys; curr_y <= bound.ye;){
            auto v = subBlock(RayBound{.xs = prev, .xe = curr, .ys = prev_y, .ye = curr_y}, depth - 1);
            output.insert(output.end(), v.begin(), v.end());
            prev_y = curr_y + 1;
            curr_y += y_step + !y_q;
        }
        prev = curr + 1;
        curr += x_step + !x_q;
    }
    return output;
}

std::vector<RayBound> split(int width, int height){
    int cnt = 4;
    int step = height / 4;
    int start = 0;
    std::vector<RayBound> output;
    for (int i = 0; i < 4; ++i) {
        output.push_back(RayBound{.xs = 0, .xe = width - 1, .ys = start, .ye = (start + step - 1) % height});
        start += step;
    }
//    output.push_back({.xs = 0, .xe = width - 1, .ys = 0, .ye = height - 1});
//    return subBlock(RayBound{.xs = 0, .xe = width, .ys = 0, .ye = height}, 0);
    return output;
}

void SceneManager::trace(){
    img.fill(Qt::black);
    auto v = split(width, height);
    std::vector<RayThread*> threads;
    auto cam = camers[curr_camera];
    auto origin = cam.position;
    auto mat = cam.viewMatrix() * cam.projectionMatrix;
    auto inverse = Mat4x4f::Inverse(mat);
    for (auto& bound: v){
        auto th = new RayThread(&cam, img,  models, inverse, bound, width, height);
        th->start();
        qDebug() << "started";
        threads.push_back(th);
    }

    for (auto& th: threads){
        th->wait();
        delete th;
    }
    show();
}
