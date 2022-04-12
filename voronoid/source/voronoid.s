@ #define SCREEN_WIDTH 240
@ #define SCREEN_HEIGHT 160


@ typedef struct
@ {
@     unsigned int x, y;
@ } TPoint;

@ void Voronoid (unsigned short* screen, const TPoint* points, int npoints, const unsigned short* palette);
@ r0 -> unsigned short* screen
@ r1 -> const TPoint* points
@ r2 -> int npoints
@ r3 -> const unsigned short* palette
.globl Voronoid
@ revisar los contadores de los bucles según ramon
Voronoid:
    stmdb   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}

    mov r4,r0
    mov r5,r1
    mov r6,r2
    mov r7,r3

    mov r8,#0           @ y = 0
Start_ForY:
    cmp r8, #160        @ y<SCREEN_HEIGHT
    bgt End_forY
    mov r9,#0           @ x = 0
Start_ForX:
    cmp r9, #240        @ x<SCREEN_WIDTH
    bgt End_forX

    mov r0,r5
    mov r1,r6
    mov r2,r9
    mov r3,r8
    
    bl Closest

    mov r0,r0,lsl #1    @ palette [c]
    ldrh r10,[r7,r0]
    
    strh r10,[r4]       @ *screen = palette [c]

    add r4,r4,#2        @ screen++

    add r9,r9,#1
    b Start_ForX
End_forX:

    add r8,r8,#1
    b Start_ForY
End_forY:

    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    bx lr