#include "model.h"
#include "OBJ_Loader.h"
#include "bary.h"
#include <QtDebug>

const float eps_intersect = std::numeric_limits<float>::epsilon();

bool equal(const Vec3f& a, const Vec3f& b){
    auto c = a - b;
    return fabs(c.x) < eps_intersect && fabs(c.y) < eps_intersect && fabs(c.z) < eps_intersect;
}

using Normals = std::vector<Vec3f>;
bool isUniqe(const Normals& normals, const Vec3f& n){
    for (auto& normal: normals)
        if (equal(n, normal)) return false;
    return true;
}

Vec3f smoothNormal(const Vec3f& p, std::vector<Face>& faces){
    int cnt = 0;
    Vec3f n = {0.f, 0.f, 0.f};
    Normals normals;
    for (auto& face: faces){
        if ((equal(p, face.a.pos) || equal(p, face.b.pos) || equal(p, face.c.pos))){
            n += face.normal;
            cnt++;
            normals.push_back(face.normal);
            qDebug() << "c = " << cnt;
        }
    }

    return (n / cnt).normalize();
}

Model::Model(const std::string& fileName, uint32_t uid_, const Vec3f& scale, const Vec3f& position){
    if (fileName == "")  return;
    uid = uid_;
    objl::Loader loader;
    bool l = loader.LoadFile(fileName);
    qDebug() <<"mean = " << l;
    color = {0.5, 0.5, 0.5};
    for (int i = 0; i < loader.LoadedMeshes.size(); ++i){
        objl::Mesh curMesh = loader.LoadedMeshes[i];
        for (int j = 0; j < curMesh.Vertices.size(); j++)
        {
            vertex_buffer.push_back(Vertex{
                                   Vec3f{curMesh.Vertices[j].Position.X , curMesh.Vertices[j].Position.Y, curMesh.Vertices[j].Position.Z},
                                   Vec3f{curMesh.Vertices[j].Normal.X,  curMesh.Vertices[j].Normal.Y, curMesh.Vertices[j].Normal.Z},
                                   curMesh.Vertices[j].TextureCoordinate.X, curMesh.Vertices[j].TextureCoordinate.Y,
                                   color
                               });
        }

        for (int j = 0; j < curMesh.Indices.size(); j++ )
            index_buffer.push_back(curMesh.Indices[j]);
    }

    // create faces
    for (int i = 0; i < this->index_buffer.size() / 3; i++){
        faces.push_back(
        {this->vertex_buffer[this->index_buffer[3 * i]],
                    this->vertex_buffer[this->index_buffer[3 * i + 1]],
                    this->vertex_buffer[this->index_buffer[3 * i + 2]]});
        auto& f = faces.back();
        f.normal = Vec3f::cross(f.b.pos - f.a.pos, f.c.pos - f.a.pos);
    }

//    for (auto& face: faces){
//        face.a.normal = smoothNormal(face.a.pos, faces);
//        face.b.normal = smoothNormal(face.b.pos, faces);
//        face.c.normal = smoothNormal(face.c.pos, faces);
//    }

    qDebug() << "size = " << faces.size();
    texture.load("C:\\raster\\ui_mode\\bricks.jpg");
    scale_x = scale.x;
    scale_y = scale.y;
    scale_z = scale.z;

    shift_x = position.x;
    shift_y = position.y;
    shift_z = position.z;

//    m_boundingBall = getBoundingBall();

}

Vertex transform_position(const Vertex& v, const Mat4x4f& objToWorld, const Mat4x4f& rotationMatrix){
    Vec4f res(v.pos);
    res = res * objToWorld;
    Vertex out = v;
    Vec4f normal(out.normal);
    normal = normal * rotationMatrix;
    out.normal = {normal.x, normal.y, normal.z};
    out.pos = Vec3f(res.x, res.y, res.z);
    return out;
}

bool Model::triangleIntersect(const Face& face, const Ray &ray, const Mat4x4f &objToWorld, const Mat4x4f &rotMatrix, InterSectionData &data){
    auto p0 = transform_position(face.a, objToWorld, rotMatrix);
    auto p1 = transform_position(face.b, objToWorld, rotMatrix);
    auto p2 = transform_position(face.c, objToWorld, rotMatrix);

    auto edge1 = p1.pos - p0.pos;
    auto edge2 = p2.pos - p0.pos;

    auto h = Vec3f::cross(ray.direction, edge2);
    auto a = Vec3f::dot(edge1, h);

    bool intersected = false;

    if (fabs(a) < eps_intersect)
        return intersected;

    auto f = 1.f / a;
    auto s = ray.origin - p0.pos;

    auto u = f * Vec3f::dot(s, h);

    if (u < 0.f || u > 1.f)
        return intersected;

    auto q = Vec3f::cross(s, edge1);

    auto v = f * Vec3f::dot(ray.direction, q);

    if (v < 0.f || u + v > 1.f)
        return intersected;

    float t = f * Vec3f::dot(edge2, q);

    if (t > eps_intersect){
        auto bary = Vec3f{1 - u - v, u, v};
//        auto p = ray.origin + ray.direction * t;
//        auto myBary = toBarycentric(p0.pos, p1.pos, p2.pos, p);
        data.normal = baryCentricInterpolation(p0.normal, p1.normal, p2.normal, bary).normalize();
        data.t = t;
        intersected = true;
        if (this->has_texture){
            float pixel_u = interPolateCord(p0.u , p1.u, p2.u, bary);
            float pixel_v = interPolateCord(p0.v, p1.v, p2.v, bary);

            int x = std::floor(pixel_u * (texture.width()) - 1);
            int y = std::floor(pixel_v * (texture.height() - 1));

            if (x < 0) x = 0;
            if (y < 0) y = 0;

            auto color = texture.pixelColor(x, y);
            auto red = (float)color.red();
            auto green = (float)color.green();
            auto blue = (float)color.blue();
            data.color = Vec3f{red / 255.f,
                    green/ 255.f ,
                    blue /255.f};

        }else
            data.color = baryCentricInterpolation(p0.color, p1.color, p2.color, bary);
//        data.color =data.normal;
    }
    return intersected;
}

bool Model::intersect(const Ray &ray, InterSectionData &data){
    float model_dist = std::numeric_limits<float>::max();

    bool intersected = false;

//    if (!m_boundingBall.intersect(ray)) return intersected;

//    qDebug() << "face new";
    auto objToWorld = this->objToWorld();
    auto rotMatrix = this->rotation_matrix;
    int cnt = 0;
    for (auto& face: faces){
        InterSectionData d;
        if (triangleIntersect(face, ray, objToWorld, rotMatrix, d) && d.t < model_dist){
            model_dist = d.t;
            data = d;
            intersected = true;
        }
    }

    return intersected;
}


NonhierSphere Model::getBoundingBall() const{
    Vec3f C = vertex_buffer[0].pos + (vertex_buffer[1].pos - vertex_buffer[0].pos) * 0.5;
    float radius = (vertex_buffer[1].pos - vertex_buffer[0].pos).len() / 2.f;

      // This is the bouncing ball algorithm. Take any two points and construct the sphere's center
      // at the center point of the line between the two vertices and raidus half the distance
      // between the two vertices
      // Then loop through each vertex. If it is outside the sphere then move the sphere's center
      // a little bit towards the point while growing the radius as well so that the point is now inside
      // the sphere. Keep doing this until there are no vertices outside the sphere
      for(bool outsider = true; outsider == true;)
      {
        outsider = false;
        for(auto v : vertex_buffer)
        {
          float length = (v.pos-C).len();
          if(length > radius)
          {
            float diff = length - radius;
            float delta = diff / length;
            C = C + (v.pos-C)*(0.5 * delta);
            radius = radius + (0.5 * diff);
            outsider = true;
          }
        }
      }

      return NonhierSphere(C, radius);
}
