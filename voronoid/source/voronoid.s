@ #define SCREEN_WIDTH 240
@ #define SCREEN_HEIGHT 160


@ typedef struct
@ {
@     unsigned int x, y;
@ } TPoint; sizeof() = 8

@ void Voronoid (unsigned short* screen, const TPoint* points, int npoints, const unsigned short* palette);
@ r0 -> unsigned short* screen
@ r1 -> const TPoint* points
@ r2 -> int npoints
@ r3 -> const unsigned short* palette
.globl Voronoid
@ .extern Closest

@ Call implementation
@ Voronoid:
    stmdb   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}

    mov r4,r0
    mov r5,r1
    mov r6,r2
    mov r7,r3

    mov r8,#0           @ y = 0
@ Start_ForY:
    cmp r8, #160        @ y<SCREEN_HEIGHT
    bge End_forY
    mov r9,#0           @ x = 0
@ Start_ForX:
    cmp r9, #240        @ x<SCREEN_WIDTH
    bge End_forX

    mov r0,r5
    mov r1,r6
    mov r2,r9
    mov r3,r8

    bl Closest

    mov r0,r0,lsl #1    @ palette [c]
    ldrh r10,[r7,r0]
    
    strh r10,[r4]       @ *screen = palette [c]

    add r4,r4,#2        @ screen++

    add r9,r9,#1        @ x++
    b Start_ForX
@ End_forX:

    add r8,r8,#1        @ y++
    b Start_ForY
@ End_forY:

    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    bx lr








@ Inline implementation
Voronoid:
    stmdb   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}

    mov r4,#0               @ y = 0
    mov r14,r1
Start_ForY:
    cmp r4, #160            @ y<SCREEN_HEIGHT
    bge End_forY
    mov r5,#0               @ x = 0
Start_ForX:
    cmp r5, #240            @ x<SCREEN_WIDTH
    bge End_forX

    mov r1,r14
@ Closest() {
    mov r11,#0              @ int closest = 0;
    ldr r7,[r1]             @ points_array[0].x
    sub r8,r7,r5            @ int xd = points_array[0].x - x
    ldr r7,[r1,#4]          @ points_array[0].y
    sub r9,r7,r4            @ int yd = points_array[0].y - y

    mul r7,r8,r8            @ xd * xd
    mul r10,r9,r9           @ yd * yd
    add r7,r7,r10           @ min_dist = xd * xd + yd * yd
    add r1,r1,#8
    mov r6,#1               @ i = 1
Start_ForI:
    cmp r6,r2               @ i < npoints
    bge End_ForI

    ldr r10,[r1]            @ points_array[i].x
    sub r8,r10,r5           @ xd = points_array[i].x - x
    ldr r10,[r1,#4]         @ points_array[i].y
    sub r9,r10,r4           @ yd = points_array[i].y - y
    
    mul r12,r8,r8           @ xd * xd
    mul r10,r9,r9           @ yd * yd
    add r10,r12,r10         @ int dist = xd * xd + yd * yd;
    
    cmp r10,r7              @ if (dist < min_dist)
    movlt r7,r10            @ min_dist = dist;
    movlt r11,r6            @ closest = i;

    add r1,r1,#8
    add r6,r6,#1            @ i++
    b Start_ForI
End_ForI:

    add r11,r3,r11,lsl #1   @ palette [c]
    ldrh r11,[r11]

@ return closest; }

    strh r11,[r0],#2        @ *screen = palette [c]; screen++;

    add r5,r5,#1            @ x++
    b Start_ForX
End_forX:

    add r4,r4,#1            @ y++
    b Start_ForY
End_forY:

    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    bx lr