
// Simple bump mapping

//#include <gba_base.h>
//#include <gba_video.h>
//#include <gba_systemcalls.h>
#include "gba_interrupt.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define GBA_SCREEN_WIDTH  (240)
#define GBA_SCREEN_HEIGHT (160)

extern const signed short bump [GBA_SCREEN_WIDTH * GBA_SCREEN_HEIGHT];
extern const unsigned char light [512 * 512];
extern const unsigned short pal [256];

static unsigned int FastAsm [128];

// GBA's graphics chip is controled by registers located at 0x4000000 
static volatile unsigned int* video_regs = (volatile unsigned int*) 0x4000000; // video registers address
// GBA's VRAM is located at address 0x6000000. 
// Screen memory in MODE 4 is located at the same place
static volatile unsigned char* screen0 = (volatile unsigned char*)0x06000000;
static volatile unsigned char* screen1 = (volatile unsigned char*)0x0600a000;

// ---------------------------------------------------------------------------

static int screen_buffers = 0;

unsigned char* GetBackBuffer()
{
  if (screen_buffers & 1)
    return (unsigned char*)screen0;
  else 
    return (unsigned char*)screen1;
}

void Flip ()
{
  screen_buffers++;
} 

void VBlankIntr (void)
{
  if (screen_buffers & 1) 
    video_regs[0] |= 0x10;
  else
    video_regs[0] &= ~0x10;
}

// ---------------------------------------------------------------------------

static void BumpMap (unsigned char* dst,
                     const signed short* bump_src, 
                     const unsigned char* light_src)
{
  unsigned short* dst16 = (unsigned short*) dst; 
  int x, y;
  for (y = 0; y < GBA_SCREEN_HEIGHT; y++) {
    for (x = 0; x < GBA_SCREEN_WIDTH; x+=2) {
      int offs = *bump_src++;
      int l0 = light_src[offs];
      light_src++; 
      offs = *bump_src++;
      int l1 = light_src[offs];
      light_src++;
      *dst16++ = l0 | (l1 << 8);
    }
    light_src+= 512 - GBA_SCREEN_WIDTH;
  }
}


int main()
{
  int i;

  // Palette memory 
  volatile unsigned short* palette = (unsigned short*)0x5000000;
  // Configure the screen at mode4, bg2 on (8 bits LUT palette based) 
  video_regs[0] = 0x404; // mode4, bg2 on

  // Fill the palette with grey scale colors
  for (i=0; i<256; i++)
    palette[i] = pal[i];

  // Copy inner loop to fast memory
  memcpy(FastAsm, BumpMap, 256); 
  void (*fun_ptr)(unsigned char* dst,const signed short* bump_src, const unsigned char* light_src) = (void*)FastAsm;

#if 1

  // SIMPLE VERSION WITHOUT INTERRUPTS

  float a = 0.0f;
  while(1) { 

    // Swap frame buffers
    volatile unsigned char* t = screen0;
    screen0 = screen1;
    screen1 = t;

    int u = (int)(256.0f - 120.0f + sin(a * 3.0f) * 100.0f);
    int v = (int)(256.0f - 80.0f + cos(a * 1.0f) * 100.0f);
    a += 0.05f;
    unsigned char* l = (unsigned char*)&light [u + v * 512];
    fun_ptr ((unsigned char*)screen0, bump, l);

    video_regs[0] ^= 0x10;
  }

#else

  // VERSION USING CORRECT DOUBLE BUFFER SYNCHRONISM (vblank interrupt)
  // Set up the interrupt handlers
  irqInit();
  // Enable Vblank Interrupt to allow VblankIntrWait
  irqEnable(IRQ_VBLANK);
  // Setup an interrupt synched to the display's verticval blank
  irqSet(IRQ_VBLANK, VBlankIntr);

  float a = 0.0f;
  while(1) { 

    int u = (int)(256.0f - 120.0f + sin(a * 3.0f) * 100.0f);
    int v = (int)(256.0f - 80.0f + cos(a * 1.0f) * 100.0f);
    a += 0.05f;
  
    unsigned char* l = (unsigned char*)&light [u + v * 512];
    fun_ptr (GetBackBuffer(), bump, l);

    Flip ();    
  }
#endif
}


