

#include <simd/simd.h>
#include <metal_stdlib>

# define SHADER_PROGRAM
#include "../../includes/SharedStructs/SharedStructs.h"

using namespace metal;

using color = Vec3;

color raytrace(thread const Ray ray) {
  Vec3 unitDir = unit_vector(ray.dir);
  float a = 0.5f * (unitDir.y() + 1.0f);
  return (1.0f - a) * color(1.0f, 1.0f, 1.0f) + a * color(0.5f, 0.7f, 1.0f);
}

kernel void computeMain(
    device const CameraMetadata*    camMetadata             [[buffer(0)]],
  	device const Vec3*	     		    vertexBuffer            [[buffer(1)]],
    texture2d<float, access::write> outputTexture						[[texture(0)]],
    uint2                           tid                     [[thread_position_in_grid]]
) {
    
    float screenWidth = outputTexture.get_width();
    float screenHeight = outputTexture.get_height();

    Vec3 pixelCenter = camMetadata->pixel00Loc + (tid.x * camMetadata->pixelDeltaU) + (tid.y * camMetadata->pixelDeltaV);
    Vec3 rayDirection = pixelCenter - camMetadata->camCenter;

    Ray ray(camMetadata->camCenter, rayDirection);

    // Normalize coordinates to [0.0, 1.0]
    // float r = (float)tid.x / screenWidth;
    // float g = (float)tid.y / screenHeight;
    // float b = 0.0f;

    color raytracedColor = raytrace(ray);
    float4 color = float4(raytracedColor.x(), raytracedColor.y(), raytracedColor.z(), 1.0f);
    outputTexture.write(color, tid);
}
