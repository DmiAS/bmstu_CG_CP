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

Vec3f RayThread::computeLightning(const Vec3f &p, const Vec3f &n, const Vec3f &direction, float specular, int depth){
    Vec3f i;
    Vec3f lightDir;
    specular = 0.6f;
    float di = 0.3f;

    float distance = 0.f;

    Vec3f ambient, diffuse = {0.f, 0.f, 0.f}, spec = {0.f, 0.f, 0.f};

    Vec3f reflect_dir = reflect(-direction, n).normalize();

    Vec3f reflect_orig = Vec3f::dot(reflect_dir, n) < 0 ? p - n * 1e-3 : p + n * 1e-3;
    Vec3f reflect_color = cast_ray(Ray(reflect_orig, reflect_dir), depth + 1);

    for (auto &model: models){
        if (model->isObject()) continue;
        Light* light = dynamic_cast<Light*>(model);
        if (light->t == Light::light_type::ambient)
//            i += light->color_intensity;
            ambient = light->color_intensity;
        else{
            float t_max;
            if (light->t == Light::light_type::point){
//                L = Vec3f{light->position.x - p.x,
//                     light->position.y - p.y,
//                     light->position.z - p.z,
//                    }.normalize();
//                t_max = 1;
                lightDir = (light->position - p);
                distance = lightDir.len();
                lightDir = lightDir.normalize();
//                L = (p - light->position).normalize();
            } else{
                lightDir = light->direction;
                t_max = std::numeric_limits<float>::max();
            }

            auto tDot = Vec3f::dot(lightDir, n);

            Vec3f shadow_orig = tDot < 0 ? p - n*1e-3 : p + n*1e-3; // checking if the point lies in the shadow of the lights[i]
////            Material tmpmaterial;
            InterSectionData tmpData;
            if (sceneIntersect(Ray(shadow_orig, lightDir), tmpData))
                if ((tmpData.point - shadow_orig).len() < distance)
                    continue;

            diffuse = (light->color_intensity * std::max(0.f, Vec3f::dot(n, lightDir)) * di);
            auto r = reflect(lightDir, n);
            auto r_dot = Vec3f::dot(r, -direction);
            auto power = powf(std::max(0.f, r_dot), 20);
            spec = light->color_intensity * power * specular;

//            Vec3f shadow_orig = Vec3f::dot(L, n) < 0 ? p - n*1e-3 : p + n*1e-3; // checking if the point lies in the shadow of the lights[i]
//            InterSectionData tmpData;
//            if (sceneIntersect(Ray(shadow_orig, L), tmpData, 1e-3, t_max) && (tmpData.point - shadow_orig).len() < L.len())
//                continue;

//            auto reflectDir = reflect(-L, n).normalize();
//            auto tmp = Vec3f::dot(-direction, reflectDir);
//            float sp = std::pow(std::fmax(tmp, 0.f), 80);
//            spec = light->color_intensity * sp * specular;
//            InterSectionData
//            auto res = closestIntersection(p, L, 1e-3, t_max);
//            if (fabs(res.t) < eps_float)
//                continue;

//            i += light->color_intensity * std::max(0.f, Vec3f::dot(n, -L));

//            Vec3f R = reflect(L, n);
//            i += (light->color_intensity * std::pow(std::max(0.f, Vec3f::dot(R, -direction)), 12500)) * specular;

//            Vec3f R = (n * Vec3f::dot(n, L) * 2.f).normalize() - L;
//            specular = powf(Vec3f::dot(-direction, R), 10);

//            float n_dot_l = Vec3f::dot(n, L);
//            if (n_dot_l > 0){
//                float factor = n_dot_l / (n.len() * L.len());
//                i = i + (light->color_intensity * factor);
//            }

//////            mirroing
//            if (!(fabs(specular + 1)< eps_float)){
//                auto r = (n * 2 * Vec3f::dot(n, L)).normalize() - L;
//                float r_dot_v = Vec3f::dot(r, direction);
//                if (r_dot_v > 0){
//                    auto len = r.len() * direction.len();
//                    auto factor = pow(r_dot_v / len, specular);
//                    i = i + light->color_intensity * factor;
//                }
//            }
        }

    }

    return (ambient + diffuse + spec + reflect_color).saturate();
}

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
    float closeset_t = std::numeric_limits<float>::infinity();
    bool intersected = false;
    for (auto& model: models){
        if (!model->isObject()) continue;
        if (model->intersect(ray, data) && data.t < closeset_t){
            closeset_t = data.t;
            intersected = true;
            data.point = ray.origin + ray.direction * data.t;
            data.model = *model;
        }
    }

    return intersected;
}


Vec3f refract(const Vec3f &I, const Vec3f &N, const float eta_t, const float eta_i=1.f) { // Snell's law
    float cosi = - std::max(-1.f, std::min(1.f, Vec3f::dot(I, N)));
    if (cosi<0) return refract(I, -N, eta_i, eta_t); // if the ray comes from the inside the object, swap the air and the media
    float eta = eta_i / eta_t;
    float k = 1 - eta*eta*(1 - cosi*cosi);
    return k<0 ? Vec3f(1,0,0) : I*eta + N*(eta*cosi - sqrtf(k)); // k<0 = total reflection, no ray to refract. I refract it anyways, this has no physical meaning
}

Vec3f RayThread::cast_ray(const Ray &ray, int depth){
    float t_min = 1e-3, t_max = std::numeric_limits<float>::max();
    InterSectionData data;
    if (depth > 1 || !sceneIntersect(ray, data))
        return Vec3f{0, 0, 0};

    auto local_color = data.color.hadamard(computeLightning(data.point, data.normal, -ray.direction, data.model.specular)).saturate();
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
        output.push_back(RayBound{.xs = 0, .xe = width - 1, .ys = start, .ye = (start + step - 1) % width});
        start += step;
    }
//    return subBlock(RayBound{.xs = 0, .xe = width, .ys = 0, .ye = height}, 0);
    return output;
}

void SceneManager::trace(){
    img.fill(Qt::black);
    auto v = split(width, height);
    std::vector<RayThread*> threads;
    auto cam = camers[curr_camera];
    auto origin = cam.position;
    auto mat = cam.viewMatrix();
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
