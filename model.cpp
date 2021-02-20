#include "model.h"
#include "OBJ_Loader.h"
#include "bary.h"
#include <QtDebug>

Model::Model(const std::string& fileName, uint32_t uid_){
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

    texture.load("C:\\raster\\ui_mode\\bricks.jpg");

    m_boundingBall = getBoundingBall();
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

bool Model::intersect(const Ray &ray, Intersection &j) const{
    bool intersected = false;

  // Check if bounding ball has been intersected first
  // If not then the mesh cannot have been intersected
   Intersection k;
   bool bball_intersected = m_boundingBall.intersect(ray, k);
   if(bball_intersected)
   {
     // Loop through each face and check if there is an intersection
     float epsilon = std::numeric_limits<float>::epsilon();
     float prev_t = std::numeric_limits<float>::infinity();
     auto objToWorld = this->objToWorld();
     auto rotMatrix = this->rotation_matrix;
     for (int i = 0; i < index_buffer.size() / 3; i++){
         auto p0 = transform_position(vertex_buffer[index_buffer[3 * i]], objToWorld, rotMatrix);
         auto p1 = transform_position(vertex_buffer[index_buffer[3 * i + 1]], objToWorld, rotMatrix);
         auto p2 = transform_position(vertex_buffer[index_buffer[3 * i + 2]], objToWorld, rotMatrix);

         auto edge1 = p1.pos - p0.pos;
         auto edge2 = p2.pos - p0.pos;

         auto h = Vec3f::cross(ray.direction, edge2);
         auto a = Vec3f::dot(edge1, h);

         if (fabs(a) < epsilon)
             continue;

         auto f = 1.f / a;
         auto s = ray.origin - p0.pos;

         auto u = f * Vec3f::dot(s, h);

         if (u < 0.f || u > 1.f)
             continue;

         auto q = Vec3f::cross(s, edge1);

         auto v = f * Vec3f::dot(ray.direction, q);

         if (v < 0.f || u + v > 1.f)
             continue;

         float t = f * Vec3f::dot(edge2, q);

         if (t < 0 || prev_t < t) continue;

         auto normal = baryCentricInterpolation(p0.normal, p1.normal, p2.normal, Vec3f{u, v, 1 - u - v});
         intersected = true;
         prev_t = t;
         j.n = normal;
     }
   }

   return intersected;
}

const float eps_intersect = std::numeric_limits<float>::epsilon();
std::pair<data_intersect, data_intersect> Model::interSect(const Vec3f& origin, const Vec3f& direction){
    float t1 = std::numeric_limits<float>::max();
    float t2 = t1;

    Vec3f n1, n2;

    auto objToWorld = this->objToWorld();
    auto rotMatrix = this->rotation_matrix;
    int cnt = 0;
    for (int i = 0; i < index_buffer.size() / 3; i++){
        auto p0 = transform_position(vertex_buffer[index_buffer[3 * i]], objToWorld, rotMatrix);
        auto p1 = transform_position(vertex_buffer[index_buffer[3 * i + 1]], objToWorld, rotMatrix);
        auto p2 = transform_position(vertex_buffer[index_buffer[3 * i + 2]], objToWorld, rotMatrix);

        auto edge1 = p1.pos - p0.pos;
        auto edge2 = p2.pos - p0.pos;

        auto h = Vec3f::cross(direction, edge2);
        auto a = Vec3f::dot(edge1, h);

        if (fabs(a) < eps_intersect)
            continue;

        auto f = 1.f / a;
        auto s = origin - p0.pos;

        auto u = f * Vec3f::dot(s, h);

        if (u < 0.f || u > 1.f)
            continue;

        auto q = Vec3f::cross(s, edge1);

        auto v = f * Vec3f::dot(direction, q);

        if (v < 0.f || u + v > 1.f)
            continue;

        float t = f * Vec3f::dot(edge2, q);

        if (t > eps_intersect){
            auto normal = baryCentricInterpolation(p0.normal, p1.normal, p2.normal, Vec3f{u, v, 1 - u - v});
            if ( t1 < std::numeric_limits<float>::max()){
                t2 = t;
                n2 = normal;
                cnt += 1;
            }
            else{
                t1 = t;
                n1 = normal;
                cnt += 1;
            }
        }
    }

    return {{t1, n1}, {t2, n2}};

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
