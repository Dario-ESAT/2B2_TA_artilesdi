@ #define SCREEN_W  (240)
@ #define SCREEN_H  (160)

@ #define CONWAY_W  (128)
@ #define CONWAY_H  (128)

@ typedef struct
@ {
@   unsigned char*  prev;   s(4)
@   unsigned char*  curr;   s(4)
@   unsigned char*  next;   s(4)
@   unsigned char*  output; s(4)
@   unsigned short* screen; s(4)
@ } tconway_line;           s(20)

@ static void SolveLine (tconway_line* line)
@ r0 -> tconway_line* line

.globl SolveLine

SolveLine:
    stmdb   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    ldr r1,[r0]             @ line->prev
    ldr r2,[r0,#4]          @ line->curr
    ldr r3,[r0,#8]          @ line->next
    ldr r4,[r0,#12]         @ line->output
    ldr r5,[r0,#16]         @ line->screen

    mov r6,#0
    strh r6,[r5]            @ line->screen[0] = 0; // El 1er pixel se ignora

    ldrb r6,[r2]            @ line->curr[0]
    strb r6,[r4]            @ line->output[0] = line->curr[0];

    mov r6,#1               @ x = 1
Start_of_for:
    cmp r6,#127             @ x < (CONWAY_W - 1)
    bge End_for
    
    sub r7,r6,#1            @ x - 1
    add r10,r6,#1           @ x + 1

    @ neighbours  = line->prev [x - 1] + line->prev [x] + line->prev [x + 1];
    ldrb r8,[r1,r7]         @ line->prev [x - 1]

    ldrb r9,[r1,r6]         @ line->prev [x - 1] + line->prev [x]
    add r8,r8,r9

    ldrb r9,[r1,r10]        @ line->prev [x - 1] + line->prev [x] + line->prev [x + 1]
    add r8,r8,r9

    @ neighbours += line->curr [x - 1]                  + line->curr [x + 1];
    ldrb r9,[r2,r7]         @ line->curr [x - 1]
    add r8,r8,r9

    ldrb r9,[r2,r10]        @ line->curr [x - 1] + line->curr [x + 1]
    add r8,r8,r9
    
    @ neighbours += line->next [x - 1] + line->next [x] + line->next [x + 1];
    ldrb r9,[r3,r7]         @ line->prev [x - 1]
    add r8,r8,r9

    ldrb r9,[r3,r6]         @ line->prev [x - 1] + line->prev [x]
    add r8,r8,r9

    ldrb r9,[r3,r10]        @ line->prev [x - 1] + line->prev [x] + line->prev [x + 1]
    add r8,r8,r9


    ldrb r7,[r2,r6]         @ int alive = line->curr [x];
    cmp r7,#0               @ if (alive) {
    bne Is_alive_curr       @ if (alive) false

    cmp r8,#3
    moveq r7,#1
    b Continue_on_curr
Is_alive_curr:              @ if (alive) true
    
    cmp r8,#2
    blt True_
    cmp r8,#3
    movgt r7,#0
    b Out_
True_:
    mov r7,#0
Out_:

Continue_on_curr:

    strb r7,[r4,r6]         @ line->output[x] = alive;

    cmp r7,#0
    moveq r7,#0
    movne r7,#0xff
    orrne r7,r7,#0x7f00
    strh r7,[r5],#2         @ line->screen[x] = alive ? 0x7fff : 0x0;

    add r6,r6,#1
    b Start_of_for
End_for:
    ldrb r9,[r2,r6]         @ line->curr[x]
    strb r9,[r4,r6]         @ line->output[x] = line->curr[x];
    mov r8,r6,lsl #1
    mov r7,#0
    strb r7,[r5,r8]         @ line->screen[x] = 0;

    mov r6,r6,lsl #1
    mov r7,#0
    strh r7,[r5,r6]         @ line->screen[x] = 0;
    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    bx lr

