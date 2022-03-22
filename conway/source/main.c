
// El juego de la vida de Conway
// Ejemplo basico de "cellular automata"

#include <gba_base.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>
#include <malloc.h>

#define SCREEN_W  (240)
#define SCREEN_H  (160)

#define CONWAY_W  (128)
#define CONWAY_H  (128)

#define RGB16(r,g,b)  ((r)+((g)<<5)+((b)<<10)) 

static unsigned char* conway_a;
static unsigned char* conway_b;

typedef struct
{
  unsigned char*  prev; 
  unsigned char*  curr; 
  unsigned char*  next;
  unsigned char*  output; 
  unsigned short* screen;
} tconway_line;


static void SolveLine (tconway_line* line)
{
  int x = 0;
  line->screen[0] = 0; // El 1er pixel se ignora
  line->output[0] = line->curr[0];

  for (x = 1; x < (CONWAY_W - 1); x++) {
    // Contar cuantas celdas de alrededor estan vivas 
    // int neighbours = 0;
    // neighbours  = line->prev [x - 1] + line->prev [x] + line->prev [x + 1];
    // neighbours += line->curr [x - 1]                  + line->curr [x + 1];
    // neighbours += line->next [x - 1] + line->next [x] + line->next [x + 1];
    int alive = line->curr [x];
    // if (alive) {   
    //   // Vivo: si tiene pocos vecinos o demasiados, morir
    //   if ((neighbours < 2) || (neighbours > 3))
    //       alive = 0; 
    // } else {
    //   // Muerto: si tiene 3 vecinos, renace
    //   if (neighbours == 3)
    //       alive = 1;
    // } 
    // Escribir resultado en el siguiente grid
    line->output[x] = alive;
    // Pintar un pixel en la pantalla, negro o blanco
    line->screen[x] = alive ? 0x7fff : 0x0;    
  }
  // Ultimo pixel de la linea, ignorar
  line->output[x] = line->curr[x];
  line->screen[x] = 0;

}


static void GameOfLife (unsigned char* prev_grid, unsigned char* curr_grid, 
                          unsigned short* screen)
{
  tconway_line lines = {0,0,0,0,0};
  int y = 0;
  for (y = 0; y < CONWAY_H; y++) {
    int y_prev = y - 1;
    int y_next = y + 1;
    // Si estamos en la primera linea, la previa es la ultima
    if (y == 0)
        y_prev = CONWAY_H - 1;
    // Si estamos en la ultima linea, la siguientes la primera
    if (y == (CONWAY_H - 1))
        y_next = 0;
    // Punteros a la linea anterior, actual y siguiente
    lines.prev = prev_grid + y_prev * CONWAY_W;
    lines.curr = prev_grid + y * CONWAY_W;
    lines.next = prev_grid + y_next * CONWAY_W;

    // Resolver el juego de la vida para las 3 lineas actuales
    lines.output = curr_grid;
    lines.screen = screen;

    SolveLine (&lines);

    // Siguiente linea en grid y pantalla
    curr_grid += CONWAY_W;
    screen += SCREEN_W;
  }
}


// Funcion pseudorandom rapida
// Esta funcion es mas rapida que la "rand" aportada por stdlib

static unsigned int PseudoRand()  
{   
  static unsigned int x = 0xdeadbeef, y = 0xbadcafe, z = 0xcafebad, w = 0x12345678;
  unsigned int t = x ^ (x << 11);
  x = y; y = z; z = w;
  return w = w ^ (w >> 19) ^ t ^ (t >> 8);
}



int main()
{
	int i = 0;  

	// Set up the interrupt handlers
	irqInit();
	// Enable Vblank Interrupt to allow VblankIntrWait
	irqEnable(IRQ_VBLANK);
 
	// Allow Interrupts
	REG_IME = 1;

    // Screen init
	volatile unsigned short* Screen = (unsigned short*)0x6000000; 
	*(volatile unsigned int*)0x4000000 = 0x403; // mode3, bg2 on (16 bits RGB)

  conway_a = (unsigned char*) malloc(SCREEN_W * SCREEN_H);
  conway_b = (unsigned char*) malloc(SCREEN_W * SCREEN_H);

  // Inicializar tablero para el "Conway's game of life"
  for (i = 0; i < SCREEN_W * SCREEN_H; i++)
      conway_a[i] = PseudoRand() & 1, conway_b[i] = 0;

	while(1) {
      // Fill scren with game of life
      VBlankIntrWait();
      GameOfLife (conway_a, conway_b, (unsigned short*)Screen);

      // Swap grids
      VBlankIntrWait();
      GameOfLife (conway_b, conway_a, (unsigned short*)Screen);
    }
}


