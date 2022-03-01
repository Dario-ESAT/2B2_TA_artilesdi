
// Temporal includes 
#include "picoGL_internal.h"
#include "raster.h"
// Pico GL include
#include "picoGL.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

const int knpointsi = 20;
const float knpointsf = 20.0f;
const float PI = 3.14159265358979323846f;
static void GetRing(float radiusOut, float radiusIn, PicoContext *ctx){


    Vec2 inner_ring[knpointsi];
    Vec2 outer_ring[knpointsi];
    int i;
    float delta = PI * 2.0f / knpointsf;

    for(i = 0; i < knpointsi; i++){
        float angle = delta * (float)i;
        inner_ring[i] = (Vec2){cosf(angle) * radiusIn + 400, sinf(angle) * radiusIn + 300};
        outer_ring[i] = (Vec2){cosf(angle) * radiusOut + 400, sinf(angle) * radiusOut + 300};
    }

    for(int i = 0; i < knpointsi; i++) {
        int next = (i + 1) < knpointsi ? i + 1 : 0;
        Vec2 tri[3];
        tri[0] = inner_ring[i];
        tri[1] = inner_ring[next];
        tri[2] = outer_ring[i];
        DrawTriangle((Framebuffer*)picoglGetFramebuffer(), ctx, tri);

        tri[0] = outer_ring[i];
        tri[1] = inner_ring[next];
        tri[2] = outer_ring[next];
        DrawTriangle((Framebuffer*)picoglGetFramebuffer(), ctx, tri);

    }
}

int main (int argc, char** argv)
{
  picoglInit   (800, 600);

  picoglClearColor (0.3f, 0.5f, 1.0f, 1.0f);

  
  
  while (1) {

    picoglBegin  ();
    picoglClear  ();
    
    PicoContext ctx = {
      .culling = kCullFront,
      .blend = kNoBlend,
      .color = 0xff0000ff,
    };

    GetRing(300.0f, 250.0f, &ctx);

    picoglEnd    ();
    picoglFlush  ();
  }
  
  picoglQuit   ();
  
  return 1;
}
