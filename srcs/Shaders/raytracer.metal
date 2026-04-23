

#include <simd/simd.h>
#include <metal_stdlib>

# define SHADER_PROGRAM
#include "../../includes/SharedStructs.h"

using namespace metal;

kernel void computeMain(
  	device const Vec3*	     		    vertexBuffer            [[buffer(0)]],
    texture2d<float, access::write> outputTexture						[[texture(0)]],
    uint2                           tid                     [[thread_position_in_grid]]
) {
    
    float width = outputTexture.get_width();
    float height = outputTexture.get_height();

    // Normalize coordinates to [0.0, 1.0]
    float r = (float)tid.x / width;
    float g = (float)tid.y / height;
    float b = 0.0f;

    float4 color = float4(r, g, b, 1.0f);
    outputTexture.write(color, tid);
}
