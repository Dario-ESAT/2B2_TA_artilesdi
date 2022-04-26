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


Voronoid:
    stmdb   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}

    mov r4,#0               @ y = 0
Start_ForY:
    cmp r4, #160            @ y<SCREEN_HEIGHT
    bge End_forY
    mov r5,#0               @ x = 0
Start_ForX:
    cmp r5, #240            @ x<SCREEN_WIDTH
    bge End_forX

    
    ldr r6,[r1]             @ points_array[0]

    ldr r7,[r6]             @ points_array[0].x
    sub r8,r7,r5            @ points_array[0].x - x
    ldr r7,[r6,#4]          @ points_array[0].y
    sub r9,r7,r4            @ points_array[0].y - y

    mul r10,r8,r8           @ xd * xd
    mul r11,r9,r9           @ yd * yd
    add r10,r10,r11         @ min_dist = xd * xd + yd * yd

    mov r6,#1               @ i = 1
Start_ForI:
    cmp r6,r2               @ i < npoints
    blt End_ForI

    ldr r12,[r1,r6,lsl #2]  @ points_array[i]

    ldr r11,[r12]            @ points_array[i].x
    sub r8,r11,r5           @ points_array[i].x - x
    ldr r11,[r12,#4]        @ points_array[i].y
    sub r9,r11,r4           @ points_array[i].y - y
    
    mul r12,r8,r8           @ xd * xd
    mul r11,r9,r9           @ yd * yd
    add r11,r10,r11         @ int dist = xd * xd + yd * yd;
    
    cmp r11,r10             @ if (dist < min_dist)
    movlt r10,r11           @ min_dist = dist;
    movlt r14,r6            @ closest = i;
      
    add r6,r6,#1            @ i++
    b Start_ForI
End_ForI:


    mov r4,r4,lsl #1        @ palette [c]
    ldrh r14,[r7,r0]
    
    strh r10,[r0]           @ *screen = palette [c]

    add r0,r0,#2            @ screen++

    add r5,r5,#1            @ x++
    b Start_ForX
End_forX:

    add r4,r4,#1            @ y++
    b Start_ForY
End_forY:

    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    bx lr