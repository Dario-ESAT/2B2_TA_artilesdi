
#include <math.h>

#include "vec2.h"
#include "picoGL_types.h"
// Pico GL include
#include "picoGL.h"
// Temporal includes 
#include "raster.h"

#define PI 3.141592f

#define N_SEGMENTS  (20)

static Vec2 gInnerRing [N_SEGMENTS];
static Vec2 gOuterRing [N_SEGMENTS];

static void DrawRing (PicoContext* ctx, float inner, float outer, float cx, float cy, bool reverse, float aoff)
{
  int i;

  for (i=0; i<N_SEGMENTS; i++) {
    float a = (2.0f * PI * (float)i) / (float)N_SEGMENTS;
    float anim = a + aoff;
    gInnerRing [i] = (Vec2){cx + cosf(anim) * inner, cy + sinf(anim) * inner};
    gOuterRing [i] = (Vec2){cx + cosf(anim) * outer, cy + sinf(anim) * outer};
  }

 for (i=0; i<N_SEGMENTS; i++) {
    int next = (i + 1) < N_SEGMENTS ? i+1 : 0;
    if (!reverse) {
      Vec2 tri0[3] = {gOuterRing [i], gOuterRing [next], gInnerRing [i]};
      DrawTriangle ((Framebuffer*)picoglGetFramebuffer(), ctx, tri0);
      Vec2 tri1[3] = {gInnerRing [i], gOuterRing [next], gInnerRing [next]};
      DrawTriangle ((Framebuffer*)picoglGetFramebuffer(), ctx, tri1);
    }
    else {
      Vec2 tri0[3] = {gOuterRing [next], gOuterRing [i], gInnerRing [i]};
      DrawTriangle ((Framebuffer*)picoglGetFramebuffer(), ctx, tri0);
      Vec2 tri1[3] = {gInnerRing [next], gOuterRing [next], gInnerRing [i]};
      DrawTriangle ((Framebuffer*)picoglGetFramebuffer(), ctx, tri1);
    }
  }
}

static float t = 0.0f;

int main (int argc, char** argv)
{
  picoglInit   (800, 600);

  picoglClearColor (0.0f, 0.0f, 1.0f, 1.0f);
  
  while (1) {
    picoglClear  ();
 
    PicoContext ctx0 = {
      .culling = kCullBack,
      .blend = kNoBlend,
      .color = 0xff00ff00,
    };

    DrawRing (&ctx0, 150.0f, 240.0f, 400.0f, 300.0f, false, t);

    PicoContext ctx1 = {
      .culling = kCullFront,
      .blend = kBlendSrcAlpha,
      .color = 0x80ff0000,
    };

    DrawRing (&ctx1, 220.0f, 280.0f, 400.0f, 300.0f, true, -t);
    
    picoglSwapBuffers  ();

    t += 0.01f;
  }
  
  picoglQuit   ();
  
  return 1;
}
