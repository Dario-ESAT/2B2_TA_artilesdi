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

@ .globl SolveLine

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
    blt End_for
    
    ldrb r8,[r2,r6]         @ int alive = line->curr [x];

    ldrb r8,[r4,r6]         @ line->output[x] = alive;

    cmp r8,#0
    
    beq Is_alive            @ line->screen[x] = alive ? 0x7fff : 0x0;
    mov r8,#0
    b Continue_on
Is_alive:
    mov r8,#32767
Continue_on:
    b Start_of_for
End_for:
    ldrb r9,[r2,r6]         @ line->curr[x]
    strb r9,[r4,r6]         @ line->output[x] = line->curr[x];

    mov r6,lsl #1
    mov r7,#0
    strh r7,[r5,r6]         @ line->screen[x] = 0;
    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    bx lr

