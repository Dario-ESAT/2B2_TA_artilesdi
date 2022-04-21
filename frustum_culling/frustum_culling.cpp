
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <SDL/SDL.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


#define PI 3.1415926535f

static inline float fsrand (int range)
{
  float r = (float)(random() % range);
  return r - (float)(range/2);
}

// ---------------------------------------------------------------------------

typedef struct 
{
  unsigned int* pixels;
  int   stride;
  short w, h;
} Canvas;


static inline void Plot (Canvas* canvas, int x, int y, int color)
{
  if ((x >= 0) && (x < canvas->w))
    if ((y >= 0) && (y < canvas->h))
      canvas->pixels [x + y * canvas->stride] |= color;
}

static inline int _abs (int v) { return v<0 ? -v : v; }

void Bresenham (Canvas* canvas, int x0, int y0, int x1, int y1, int color) 
{ 
  int dx = _abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = _abs(y1-y0), sy = y0<y1 ? 1 : -1; 
  int err = (dx>dy ? dx : -dy)/2, e2;
 
  for(;;) {
    Plot (canvas, x0, y0, color);

    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}

// ---------------------------------------------------------------------------

static void DrawSphere (Canvas* canvas, glm::vec3 center, float radius, glm::vec2 screen_pos, unsigned int color)
{
  int rads = 20;
  for (int p = 0; p <= rads/2;p++) {
    float phi = -PI/2.0f + (PI * ((float)p) / (float)(rads/2));
    for (int t = 0; t < rads; t++) {    // meridianos
      float theta = 2.0f * PI * (((float)t) / (float)rads);
      float x = radius * cosf(phi) * sinf(theta);
      float y = radius * sinf(phi) * sinf(theta);
      float z = radius * cosf(theta);
      
      Plot (canvas, screen_pos.x + (center.x + x), screen_pos.y - (center.z + z), color);
    }
  }
 
}

static void DrawAxis (Canvas* canvas, glm::vec2 fov_h_vector, glm::vec2 screen_pos)
{
  int ca = 0xffffff;
  int cf = 0x888800;
  float l = 300.0;
  Bresenham (canvas, screen_pos.x, screen_pos.y - l, screen_pos.x, screen_pos.y + l, ca); 
  Bresenham (canvas, screen_pos.x - l, screen_pos.y, screen_pos.x + l, screen_pos.y, ca); 

  Bresenham (canvas, screen_pos.x + fov_h_vector.y * l, screen_pos.y + fov_h_vector.x * l, 
                     screen_pos.x - fov_h_vector.y * l, screen_pos.y - fov_h_vector.x * l, cf); 

  Bresenham (canvas, screen_pos.x - fov_h_vector.y * l, screen_pos.y + fov_h_vector.x * l, 
                     screen_pos.x + fov_h_vector.y * l, screen_pos.y - fov_h_vector.x * l, cf); 

}


// ---------------------------------------------------------------------------

typedef struct
{
   glm::vec3 pos;
   float     radius;
} Sphere;


typedef struct
{
  glm::vec3 plane_left;
  glm::vec3 plane_right;
  glm::vec3 plane_top;  
  glm::vec3 plane_down;
} Frustum;


static inline bool SphereAgainstFrustum (Frustum& frustum, glm::vec3 point, float radius)
{
  // De la ecuacion general del plano
  // D = -(ax * by * Cz);
  // D es la distancia punto->plano (si el vector director es unitario)
  float D = -glm::dot(frustum.plane_right, point);
  bool inside_right = D >= -radius;

  D = -glm::dot(frustum.plane_left, point);
  bool inside_left = D >= -radius;
  
  return inside_left && inside_right;
}

// ---------------------------------------------------------------------------

#define N_SPHERES (100)

static Sphere gSpheres [N_SPHERES] = {};


int main ( int argc, char **argv)
{
  SDL_Surface  *g_SDLSrf;

  //int mouse_x = 0, mouse_y = 0;
  int req_w = 1024;
  int req_h = 768;

  // Init SDL and screen
  if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
    fprintf(stderr, "Can't Initialise SDL: %s\n", SDL_GetError());
    exit(1);
  }
  if (0 == SDL_SetVideoMode( req_w, req_h, 32,  SDL_HWSURFACE | SDL_DOUBLEBUF)) {
    printf("Couldn't set %dx%dx32 video mode: %s\n", req_w, req_h, SDL_GetError());
    return 0;
  }

  g_SDLSrf = SDL_GetVideoSurface();
  
 
  // Lista de esferas con radios y posiciones al azar
  int i;
  for (i=0; i<N_SPHERES; i++) {
    gSpheres[i].pos = glm::vec3(fsrand(700), fsrand(700), fsrand(700));
    //printf ("%f %f %f\n", gSpheres[i].pos.x,  gSpheres[i].pos.y,  gSpheres[i].pos.z);
    gSpheres[i].radius = 15 + (random() % 20);
  }
 

  glm::vec2 screen(800.0f, 600.0f);
  // Definicion del Frustum respecto al FOV horizontal
  float fov_h = 60.0f * (PI/180.0f); // Degrees to radians
 
  float half_fov_h = fov_h * 0.5f;
  // Deduccion del fov vertical
  // - distancia a pantalla
  float d = screen.x * 0.5f * (cosf (half_fov_h) / sinf (half_fov_h));
  // - angulo altura-distancia
  float va = atan2f(screen.y * 0.5f, d);
  float fov_v = 2.0f * va;
  float half_fov_v = va;
  
  // Vector 2D que representa la vista zenital del fov horizontal
  glm::vec2 fov_h_vector(cosf(half_fov_h), sinf(half_fov_h));
  // Vector 2D que representa la vista zenital del fov vertical
  glm::vec2 fov_v_vector(cosf(half_fov_v), sinf(half_fov_v));
  
  // Plano correspondiente a dicho vector
  // El vector director corresponde al vector del fov girado 90 grados a la derecha
  Frustum frustum;
  frustum.plane_right = glm::vec3 (fov_h_vector.x, 0.0f, -fov_h_vector.y);
  frustum.plane_left  = glm::vec3 (-fov_h_vector.x, 0.0f, -fov_h_vector.y);
  frustum.plane_top   = glm::vec3 (0.0f, fov_v_vector.x, -fov_v_vector.y);
  frustum.plane_down  = glm::vec3 (0.0f, -fov_v_vector.x, -fov_v_vector.y);

  // Centro display debug
  glm::vec2 offs_left((float)(req_w/2), (float)(req_h/2));
  int end = 0;
  while ( !end) { 

    SDL_Event event;

    // Lock screen to get access to the memory array
    SDL_LockSurface( g_SDLSrf);

    // Borrar pantalla
    SDL_FillRect(g_SDLSrf, NULL, SDL_MapRGB(g_SDLSrf->format, 0, 0, 0));

    Canvas canvas;
    canvas.pixels = (unsigned int*)g_SDLSrf->pixels;
    canvas.stride = g_SDLSrf->pitch >> 2;
    canvas.w = g_SDLSrf->w;
    canvas.h = g_SDLSrf->h;


    float time = 0.05f * (float)SDL_GetTicks();

    // Movemos los objetos
    glm::mat4 view = glm::mat4(1.0f); 
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f));
    view = glm::rotate(view, 0.001f * (float)time, glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::rotate(view, 0.002f * (float)time, glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, 0.003f * (float)time, glm::vec3(0.0f, 0.0f, 1.0f));

    DrawAxis (&canvas, fov_h_vector, offs_left);    
    for (i=0; i<N_SPHERES; i++) {
      glm::vec4 rotated = view * glm::vec4(gSpheres[i].pos, 1.0f);
      glm::vec3 t(rotated.x, rotated.y, rotated.z);
      
      bool inside = SphereAgainstFrustum(frustum, t, gSpheres[i].radius);
      DrawSphere (&canvas, t, gSpheres[i].radius, offs_left, inside ? 0xffffff: 0x880000);
    }

    SDL_Delay (16);    // Forma simplona (no recomendada) de evitar framerates de vertigo

    SDL_UnlockSurface( g_SDLSrf);
    SDL_Flip( g_SDLSrf);

    // Check input events
    while ( SDL_PollEvent(&event) ) {
      switch (event.type) {
        case SDL_MOUSEMOTION:
          //mouse_x = event.motion.x;
          //mouse_y = event.motion.y;
          break;
        case SDL_MOUSEBUTTONDOWN:
          //printf("Mouse button %d pressed at (%d,%d)\n",
          //       event.button.button, event.button.x, event.button.y);
          break;
        case SDL_QUIT:
          end = 1;
          break;
      }
    }
  }

  return 0;
}


