#include "raythread.h"
#include "scene_manager.h"

const float eps_float = 1e-5;
bool checkIntersection(const float& t, const float& t_min, const float& t_max, const float& closest_t){
    return t > t_min && t < t_max && t < closest_t;
}

InterSectionData RayThread::closestIntersection(const Vec3f& o, const Vec3f& d, float t_min, float t_max){
    auto closest_t = std::numeric_limits<float>::max();
    Model closest_model;
    Vec3f normal;

    for (auto& model: models){
        auto p = model->interSect(o, d);
        if (checkIntersection(p.first.first, t_min, t_max, closest_t)){
            closest_t = p.first.first;
            closest_model = *model;
            normal = p.first.second;
        }
        if (checkIntersection(p.second.first, t_min, t_max, closest_t)) {
            closest_t = p.second.first;
            closest_model = *model;
            normal = p.second.second;
        }
    }

    return {closest_model, closest_t, normal.normalize()};
}

Vec3f RayThread::computeLightning(const Vec3f &p, const Vec3f &n, const Vec3f &direction, float specular){
    Vec3f i;
    Vec3f L;


    for (auto &model: models){
        if (model->isObject()) continue;
        Light* light = dynamic_cast<Light*>(model);
        if (light->t == Light::light_type::ambient)
            i += light->color_intensity;
        else{
            float t_max;
            if (light->t == Light::light_type::point){
                L = {light->position.x - p.x,
                     light->position.y - p.y,
                     light->position.z - p.z,
                    };
                t_max = 1;
            } else{
                L = light->direction;
                t_max = std::numeric_limits<float>::max();
            }

            float shadow_t = std::numeric_limits<float>::max();
            auto res = closestIntersection(p, L, 1e-3, t_max);
            if (fabs(res.t) < eps_float)
                continue;

            float n_dot_l = Vec3f::dot(n, L);
            if (n_dot_l > 0){
                auto denum = (n.len() * L.len());
                auto factor = n_dot_l / denum;
                i.x = light->color_intensity.x * factor;
                i.y = light->color_intensity.y * factor;
                i.z = light->color_intensity.z * factor;
            }

            //mirroing
//            if (!(fabs(specular + 1)< eps_float)){
//                auto r = (n * 2 * Vec3f::dot(n, L)) - L;
//                float r_dot_v = Vec3f::dot(r, direction);
//                if (r_dot_v > 0){
//                    auto len = r.len() * direction.len();
//                    auto factor = pow(r_dot_v / len, specular);
//                    i.x = light->color_intensity.x * factor;
//                    i.y = light->color_intensity.y * factor;
//                    i.z = light->color_intensity.z * factor;
//                }
//            }
        }

    }

    return i;
}

Vec3f reflectRay(const Vec3f& r, const Vec3f& n){
    return (n * 2 * Vec3f::dot(n, r)) - r;
}

Vec3f RayThread::traceRay(const Vec3f& o, const Vec3f& d, float t_min, float t_max, int depth){
    auto res = closestIntersection(o, d, t_min, t_max); // Model - closest_t

    if (fabs(res.t -  std::numeric_limits<float>::max()) < eps_float)
        return Vec3f{0, 0, 0};
//    return res.model.color * 255.f;

    auto p = o + d * res.t;

    auto computed_light = computeLightning(p, res.normal, -d, res.model.specular);

    auto local_color = res.model.color.hadamard(computed_light);

//    float r = res.model.reflective;
//    if (depth <= 0 || r <= 0){
//        return local_color;
//    }

//    if (r >= 0){
//        auto R = reflectRay(-d, res.normal);
//        auto reflected_color = traceRay(p, R, 1e-3, std::numeric_limits<float>::max(),
//                                        depth - 1);
//        local_color = local_color * (1 - r) + reflected_color * r;
//    }

    return local_color;

//    Vec3f p = o + d * res.second;

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
    for (int i = 0; i < cnt; ++i) {
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
    auto mat = cam.viewMatrix() * cam.projectionMatrix;
    auto inverse = Mat4x4f::Inverse(mat);
    for (auto& bound: v){
        auto th = new RayThread(img, models, inverse, bound, width, height, origin);
        th->start();
        qDebug() << "started";
        threads.push_back(th);
    }

    for (auto& th: threads){
        th->wait();
        delete th;
    }

//    for (int x = 0; x < width; x++){
//        for (int y = 0; y < height; y++){
////            float px = (2 * ((float)x / (float)width) - 1) * tan(cam.fov / 2 * M_PI / 180) * cam.aspect_ratio;
////            float py = (1 - 2 * ((float)y / (float)height)) * tan(cam.fov / 2 * M_PI / 180);
////            auto d = Vec3f{px, py, 1}.normalize();
////            auto d = canvasToViewPort(x, y);
////            qDebug() << height / 2 << d.x << d.y << d.z;
////            return;
//            auto d = toWorld(x, y, inverse, width, height).normalize();
//            auto color = traceRay(origin, d, 1, std::numeric_limits<float>::max(), 1) * 255.f;
//            img.setPixelColor(x, y, QColor(color.x, color.y, color.z));
//        }
//    }
    show();
}
