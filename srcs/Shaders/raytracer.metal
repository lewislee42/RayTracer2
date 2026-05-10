

# include <simd/simd.h>
# include <metal_stdlib>

# define SHADER_PROGRAM
# include "SharedStructs/SharedStructs.h"

using namespace metal;

using color = Vec3;


bool rayIntersectionTriangle(
  Ray                     ray,
  device const Triangle*  triangles,
  float                   rayMin,
  float                   rayMax,
  thread HitRecord*       record
) {

  Vec3 edge1 = triangles[0].b - triangles[0].a;
  Vec3 edge2 = triangles[0].c - triangles[0].a;


  Vec3 normal = cross(edge1, edge2);
  if (dot(normal, ray.dir) > 0) {
    return false;
  }


  Vec3 rayCrossE2 = cross(ray.dir, edge2);
  float det       = dot(edge1, rayCrossE2);

  if (abs(det) < FLT_EPSILON) {
    return false;
  }


  float invDet  = 1.0f / det;
  Vec3  s       = ray.origin - triangles[0].a;
  float u       = invDet * dot(s, rayCrossE2);

  if (u < -FLT_EPSILON || u - 1 > FLT_EPSILON) {
    return false;
  }


  Vec3  sCrossE1  = cross(s, edge1);
  float v         = invDet * dot(ray.dir, sCrossE1);

  if (v < -FLT_EPSILON || u + v - 1 > FLT_EPSILON) {
    return false;
  }

  float t = invDet * dot(edge2, sCrossE1);

  if (t > FLT_EPSILON) {
    if (t <= rayMin || rayMax <= t) {
        return false;
    }

    record->t = t;
    record->p = ray.at(t);
    record->normal = unit_vector(cross(edge1, edge2));

    return true;
  }

  return false;
}


color raytrace(thread const Ray ray, device const Triangle* triangles) {
  HitRecord record;

  if (rayIntersectionTriangle(ray, triangles, 0, INFINITY, &record)) {

    Vec3 N = record.normal;
    return 0.5f * color(N.x() + 1, N.y() + 1, N.z() + 1);
  }

  Vec3 unitDir = unit_vector(ray.dir);
  float a = 0.5f * (unitDir.y() + 1.0f);
  return (1.0f - a) * color(1.0f, 1.0f, 1.0f) + a * color(0.5f, 0.7f, 1.0f);
}


kernel void computeMain(
    texture2d<float, access::write> outputTexture						[[texture(0)]],
    device const CameraMetadata*    camMetadata             [[buffer(0)]],
  	device const Triangle*	     		triangles               [[buffer(1)]],
    uint2                           tid                     [[thread_position_in_grid]]
) {
    
    float screenWidth = outputTexture.get_width();
    float screenHeight = outputTexture.get_height();

    Vec3 pixelCenter = camMetadata->pixel00Loc + (tid.x * camMetadata->pixelDeltaU) + (tid.y * camMetadata->pixelDeltaV);
    Vec3 rayDirection = pixelCenter - camMetadata->camCenter;

    Ray ray(camMetadata->camCenter, rayDirection);

    color raytracedColor = raytrace(ray, triangles);
    float4 color = float4(raytracedColor.x(), raytracedColor.y(), raytracedColor.z(), 1.0f);
    outputTexture.write(color, tid);
}
