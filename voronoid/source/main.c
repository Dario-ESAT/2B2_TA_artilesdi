
#include <gba_base.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>

#define RGB16(r,g,b)  ((r)+((g)<<5)+((b)<<10)) 


typedef struct
{
    unsigned int x, y;
} TPoint;

static const TPoint _points_array [] =
{
  {30,11},
  {66,125},
  {100,37},
  {222,60},
  {201,87},
  {240,158},
};

static const unsigned short _points_colors [] =
{
  RGB16(31,0,0),
  RGB16(0,31,0),
  RGB16(0,0,31),
  RGB16(0,31,31),
  RGB16(31,0,31),
  RGB16(31,31,31),
};

int Closest (const TPoint* points_array, int npoints, int x, int y)
{
  int i = 0;
  int closest = 0;    // 1st point is closest, by default
  int xd = points_array[0].x - x;
  int yd = points_array[0].y - y;
  int min_dist = xd * xd + yd * yd;
  for (i = 1; i < npoints; i++) {
    xd = points_array[i].x - x;
    yd = points_array[i].y - y;
    int dist = xd * xd + yd * yd;
    if (dist < min_dist) {
      min_dist = dist;
      closest = i;
    }
  }
  return closest;
}



// Fill the screen with closest points to each pixel


void Voronoid (unsigned short* screen, const TPoint* points, int npoints, 
               const unsigned short* palette)
{
  int x,y;
  for(y = 0; y<SCREEN_HEIGHT; y++) {
    for(x = 0; x<SCREEN_WIDTH; x++) {
      int c = Closest (points, 
              npoints, 
              x, y);
      *screen = palette [c];
      screen++;
    }
  }
}



int main()
{
	// Set up the interrupt handlers
	irqInit();
	// Enable Vblank Interrupt to allow VblankIntrWait
	irqEnable(IRQ_VBLANK);
 
	// Allow Interrupts
	REG_IME = 1;

  // GBA's VRAM is located at address 0x6000000. 
  // Screen memory in MODE 3 is located at the same place
	volatile unsigned short* screen = (unsigned short*)0x6000000;
  // GBA's graphics chip is controled by registers located at 0x4000000 
	volatile unsigned int* video_regs = (unsigned int*) 0x4000000; // mode3, bg2 on (16 bits RGB)
  // Configure the screen at mode 3 using the display mode register
	video_regs[0] = 0x403; // mode3, bg2 on (16 bits RGB)

	while(1)  {

    VBlankIntrWait();

    Voronoid ((unsigned short*) screen, _points_array, 
              sizeof(_points_array) / sizeof(TPoint), // Quantity of points in the array
              _points_colors);

  }
}


