
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#include "vec2.h"

#define WINDOW_WIDTH  (800)
#define WINDOW_HEIGHT (600)

static GLFWwindow* gWindow = 0;

static unsigned char* ReadSrcCode (const char* filename)
{
  FILE* in_file = fopen(filename, "rb");
  assert(in_file); 
  struct stat sb;
  assert(stat(filename, &sb) != -1);
  unsigned char* file_contents = (unsigned char*)malloc(sb.st_size + 1);
  fread(file_contents, sb.st_size, 1, in_file);
  file_contents [sb.st_size] = 0;
  fclose(in_file);
  return file_contents;
}

// Callback: reconfigurar OGL en caso de redimensionado de ventanas
// ------------------------------------------------------------------------------------

static void window_resize_callback (GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

// Inicializaciones: abrir ventana, etc
// ------------------------------------------------------------------------------------

static void OglInit ()
{
  // La libreria GLFW se usa como complemento de OGL para gestionar ventanas y contextos
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  gWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Rings", NULL, NULL);
  assert(gWindow);

  // Asocia el contexto de OGL actual a esta ventana; si abres varias ventanas
  // cada una puede tener un contexto OGL propio
  glfwMakeContextCurrent(gWindow);
  glfwSetFramebufferSizeCallback(gWindow, window_resize_callback);
  
  // GLEW es otra libreria de acompañamiento de OGL. Gestiona las extensiones
  // de la libreria, y es necesaria para poder operar (o una equivalente).
  GLenum error = glewInit();
  assert (error == GLEW_OK);
}


// Shaders
// ------------------------------------------------------------------------------------

static char infoLog [512];

// Imprime los posibles errores de compilacion o linkado
static void CheckGPUErrors (unsigned int code, const char* str, bool link)
{
  int success = 0;
  if (link) {
    glGetProgramiv(code, GL_LINK_STATUS, &success); 
    if (!success)
      glGetProgramInfoLog(code, 512, NULL, infoLog);
  } else {
    glGetShaderiv (code, GL_COMPILE_STATUS, &success);
    if (!success)
      glGetShaderInfoLog(code, 512, NULL, infoLog);
  }
  if (!success) {
    std::cout <<  str << infoLog << std::endl;
  }
}


static void ShadersInit (unsigned int& shader_program, 
                         const char* vertex_src, 
                         const char* shader_src)
{
  // Vertex shader
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertex_src, NULL);
  glCompileShader(vertexShader);
  CheckGPUErrors (vertexShader, "VERTEX COMPILATION_FAILED\n", false);

  // Fragment shader
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &shader_src, NULL);
  glCompileShader(fragmentShader);
  CheckGPUErrors (fragmentShader, "FRAGMENT COMPILATION_FAILED\n", false);

  // Link shaders
  shader_program = glCreateProgram();
  glAttachShader (shader_program, vertexShader);
  glAttachShader (shader_program, fragmentShader);
  glLinkProgram(shader_program);
  CheckGPUErrors (shader_program, "SHADER PROGRAM LINKING_FAILED\n", true);

  // Los shaders compilados pueden ser liberados una vez linkado el programa
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

// ------------------------------------------------------------------------------------

static void UploadMesh (float* mesh, int mesh_size, unsigned int& VBO, unsigned int& VAO)
{
  // VBO = Vertex buffer object
  // VAO = Vertex array object
  // Generamos 2 instancias de objetos OGL: array y buffer (sin inicializar)
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  // Enfocamos ambos objetos, VAO y VBO 
  // Esto es necesariom sino podriamos tocar atributos de VBOs y VAOs usados anteriormente
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  
  // Configuramos el buffer (tipo y cantidad de memoria) 
  // STATIC_DRAW significa que los datos no se modificaran en el futuro
  glBufferData(GL_ARRAY_BUFFER, mesh_size, mesh, GL_STATIC_DRAW);
  // Configuramos el array (atributos dela estructura)
  // Es un array de vertices de 3 floats, con una separacion entre ellos de 12 bytes
  // El indice de attributo 0 se refiere normalmente a vertices (X,Y,X),
  // el 2 a normales (X,Y,Z) y el 2 a coord. textura (U,V). Pero no esta recomendado
  // usarlos asi, sino con (ej,) glGetAttribLocation(program, "position")
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  // Se quita el foco del VAO y el VBO; glVertexAttribPointer ya se ha quedado las referencias
  // Quizar el foco evita que otras llamadas puedan modificar estos objetos accidentalmente 
  glBindBuffer(GL_ARRAY_BUFFER, 0); 
  glBindVertexArray(0); 
}

// ------------------------------------------------------------------------------------

#define N_SEGMENTS  (20)
#define PI 3.141592f

static Vec2 gInnerRing [N_SEGMENTS];
static Vec2 gOuterRing [N_SEGMENTS];
static float gRawMesh0 [N_SEGMENTS * 3 * 3 * 2]; // 1 tri = 3 vertex (3 floats each: x, y, z)
static float gRawMesh1 [N_SEGMENTS * 3 * 3 * 2];

// Change 2D to 3D coordinates (OGL unitary coordinates)
static inline void CopyVtx (float* f, Vec2* v)
{
  f[0] = v->x * (2.0f/(float)WINDOW_WIDTH);
  f[1] = v->y * (2.0f/(float)WINDOW_HEIGHT);
  f[2] = 0.0f;
}

static void DrawRing (float* mesh, float inner, float outer, float cx, float cy, bool reverse)
{
  int i;

  for (i=0; i<N_SEGMENTS; i++) {
    float a = (2.0f * PI * (float)i) / (float)N_SEGMENTS;
    float anim = a;
    gInnerRing [i] = (Vec2){cx + cosf(anim) * inner, cy + sinf(anim) * inner};
    gOuterRing [i] = (Vec2){cx + cosf(anim) * outer, cy + sinf(anim) * outer};
  }

  int vi = 0;
  for (i=0; i<N_SEGMENTS; i++) {
    int next = (i + 1) < N_SEGMENTS ? i+1 : 0;
    if (!reverse) {
      CopyVtx (&mesh[vi+0], &gOuterRing [i]);
      CopyVtx (&mesh[vi+3], &gOuterRing [next]);
      CopyVtx (&mesh[vi+6], &gInnerRing [i]);

      CopyVtx (&mesh[vi+9], &gInnerRing [i]);
      CopyVtx (&mesh[vi+12], &gOuterRing [next]);
      CopyVtx (&mesh[vi+15], &gInnerRing [next]);
    }
    else {
      CopyVtx (&mesh[vi+0], &gOuterRing [i]);
      CopyVtx (&mesh[vi+6], &gOuterRing [next]);
      CopyVtx (&mesh[vi+3], &gInnerRing [i]);

      CopyVtx (&mesh[vi+9], &gInnerRing [i]);
      CopyVtx (&mesh[vi+15], &gOuterRing [next]);
      CopyVtx (&mesh[vi+12], &gInnerRing [next]);
      
    }
    vi += 6 * 3;
  }
}


int main(int argc, char** argv)
{
  OglInit ();

  unsigned int shader_program = 0;
  
  unsigned char* vertexShaderSource   = ReadSrcCode ("vertex.glslv");
  unsigned char* fragmentShaderSource = ReadSrcCode ("fragment.glslf");
  
  ShadersInit (shader_program, (char*)vertexShaderSource, (char*)fragmentShaderSource); 

  DrawRing (gRawMesh0, 150.0f, 240.0f, 0.0f, 0.0, false);
  DrawRing (gRawMesh1, 220.0f, 280.0f, 0.0f, 0.0, true);

  unsigned int VBO0, VAO0;
  UploadMesh (gRawMesh0, sizeof(gRawMesh0), VBO0, VAO0);
  unsigned int VBO1, VAO1;
  UploadMesh (gRawMesh1, sizeof(gRawMesh0), VBO1, VAO1);

  glBlendFunc    (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  int location_color = glGetUniformLocation(shader_program, "RawColor");
    
  // Main loop
  while (!glfwWindowShouldClose(gWindow))
  {
    // Evento de salida
    if (glfwGetKey(gWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(gWindow, true);


    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable  (GL_CULL_FACE);
    glCullFace (GL_BACK);

    glUseProgram(shader_program);
    
    glUniform4f(location_color, 0.0f, 1.0f, 0.0f, 1.0f);

    glBindVertexArray(VAO0); 
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * N_SEGMENTS);

    glEnable  (GL_CULL_FACE);
    glCullFace (GL_FRONT);

    glEnable    (GL_BLEND);
  
    glUniform4f(location_color, 1.0f, 0.0f, 0.0f, 0.5f);
    glBindVertexArray(VAO1); 
    glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * N_SEGMENTS);

    glDisable   (GL_BLEND);

    glfwSwapBuffers(gWindow);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO0);
  glDeleteBuffers(1, &VBO0);
  glDeleteVertexArrays(1, &VAO1);
  glDeleteBuffers(1, &VBO1);
  glDeleteProgram(shader_program);

  glfwTerminate();
  return 0;
}




