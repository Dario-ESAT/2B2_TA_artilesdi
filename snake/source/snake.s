
@ typedef struct
@ {
@   int x, y;
@ } Slab;   size = 8 

@ typedef struct
@ {
@   Slab* slabs;                S(4)
@   unsigned short len;         S(2)
@   unsigned short head_slab;   S(2)
@   int   speed_x, speed_y;     S(8)
@ } Snake;  size = 16



@ int UpdateSnake (int keypad, Snake* snake, unsigned short* screen)
@ r0 -> int keypad
@ r1 -> Snake* snake
@ r2 -> unsigned short* screen

.globl UpdateSnake

UpdateSnake:
    stmdb   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}

    ldr r4,[r1]         @ Slab* slabs = snake->slabs;
    ldrh r5,[r1,#6]     @ int head_slab = snake->head_slab;

    add r6,r5,#1        @ int tail = snake->head_slab + 1;

    ldrh r7,[r1,#4]     @ if (tail >= snake->len)
    cmp r6,r7
    movge r6,#0         @ tail = 0;
    
    add r9,r4,r6,lsl #3 @ slabs + tail * sizeof(Slab)(8)

    ldr r7,[r9]         @ slabs[tail].x

    ldr r8,[r9,#4]      @ slabs[tail].y

    mov r11,#240

    mul r9,r8,r11       @ slabs[tail].y * 240
    add r7,r7,r9        @ slabs[tail].x + slabs[tail].y * 240
    mov r7,r7,lsl #1    @ slabs[tail].x + slabs[tail].y * 240 * sizeof(short)(2)
    mov r8,#0
    strh r8,[r2,r7]     @ screen[slabs[tail].x + slabs[tail].y * 240] = 0

    add r7,r4,r5,lsl #3 @ slabs + head_slab * sizeof(Slab)(8)
    
    ldr r8,[r7]         @ int new_x = slabs[snake->head_slab].x
    ldr r9,[r7,#4]      @ int new_y = slabs[snake->head_slab].y

    add r5,r5,#1        @ head_slab++

    ldrh r7,[r1,#4]     @ snake->len

    cmp r5,r7           @ if (head_slab >= snake->len)
    movge r5,#0         @ head_slab = 0
    strh r5,[r1,#6]     @ snake->head_slab = head_slab

    ldr r10,[r1,#8]     @ int speed_x = snake->speed_x
    ldr r6,[r1,#12]     @ int speed_y = snake->speed_y

    cmp r0,#0
    beq If_keypad_0
    mov r10,#0           @ speed_x = speed_y = 0
    mov r6,#0

    ands r7,r0,#1        @ if (keypad & 1)
    movne r10,#1         @ speed_x = 1

    ands r7,r0,#2        @ if (keypad & 2)
    movne r10,#-1        @ speed_x = -1

    ands r7,r0,#4        @ if (keypad & 4)
    movne r6,#-1        @ speed_y = -1

    ands r7,r0,#8        @ if (keypad & 8)
    movne r6,#1         @ speed_y = 1

    str r10,[r1,#8]     @ snake->speed_x = speed_x
    str r6,[r1,#12]     @ snake->speed_y = speed_y
If_keypad_0:
    add r8,r8,r10       @ new_x += speed_x
    add r9,r9,r6        @ new_y += speed_y

    mul r7,r9,r11       @ new_y * 240
    add r7,r7,r8        @ new_x + new_y * 240
    add r7,r2,r7,lsl #1 @ unsigned short* head_pix = screen + (new_x + new_y * 240 * sizeof(short)(2))

    ldrh r11,[r7]
    cmp r11,#0          @ if (*head_pix == 0)
    bne If_head_pix_not_0

    mov r10,#-1         @ *head_pix = 0xffff;
    strh r10,[r7]

    add r7,r4,r5,lsl #3 @ labs[head_slab * sizeof(Slab)(8)]

    str r8,[r7]         @ slabs[head_slab].x = new_x
    str r9,[r7,#4]      @ slabs[head_slab].y = new_y

    mov r0,#0
    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    bx lr
If_head_pix_not_0:
    mov r0,#1 
    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    bx lr