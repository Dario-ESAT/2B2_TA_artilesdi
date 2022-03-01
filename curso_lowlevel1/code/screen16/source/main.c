
#include <gba_base.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>

#define RGB16(r,g,b)  ((r)+((g)<<5)+((b)<<10)) 
/*
- Recuadro negro sobre un fondo blanco, desde 30,20 hasta 220, 150 el recuadro son 4 lineas negras

- Objetivo: hacerlo usando el menor numero posible de operaciones

1.- Fill completo blanco
2.- 4 lineas negras
*/

#define WHITE 0x7fff //11111 11111 11111
#define BLACK 0
#define WIDTH 240
#define HEIGHT 160

int main()
{
	int x,y = 0;
//    int g = 0;
    int i;

	// Set up the interrupt handlers
	irqInit();
	// Enable Vblank Interrupt to allow VblankIntrWait
	irqEnable(IRQ_VBLANK);
 
	// Allow Interrupts
	REG_IME = 1;

  // GBA's VRAM is located at address 0x6000000. 
  // Screen memory in MODE 3 is located at the same place
  unsigned short* screen = (unsigned short*)0x6000000;
  // GBA's graphics chip is controled by registers located at 0x4000000 
  volatile unsigned int* video_regs = (unsigned int*) 0x4000000; // mode3, bg2 on (16 bits RGB)
  // Configure the screen at mode 3 using the display mode register
  video_regs[0] = 0x403; // mode3, bg2 on (16 bits RGB)

		VBlankIntrWait();
 // Fill scren
   /*
    for(y = 0; y<160; y++) 
      for(x = 0; x<240;x++)
        screen[x + y * 240] = RGB16(y & 31, g, x & 31);

    g++;
  }*/

    for(i = 0; i < HEIGHT * WIDTH; i++){
        screen[i] = WHITE;    
    }

    unsigned short* aux = screen + 30 + 20 * WIDTH;
    for(x = 30; x <= 220;x++) {
        *aux = BLACK;
        aux++;
        
    }
    
	aux = screen + 30 + 150 * WIDTH;
	
    for(x = 30; x <= 220;x++) {
        *aux = BLACK;
        aux++;
        
    }
    
	aux = screen + 30 + 20 * WIDTH;
	
	for(y = 20; y <= 150;y++) {
        *aux = BLACK;
        aux += WIDTH;
        
    }
    
    aux = screen + 220 + 20 * WIDTH;
	
	for(y = 20; y <= 150;y++) {
        *aux = BLACK;
        aux += WIDTH;
        
    }

    
	while(1);
}


