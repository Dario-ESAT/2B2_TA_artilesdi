@ SCREEN_W (240)
@ SCREEN_H (160)

@ BALL_SIDE (16)

@ RGB16(R,G,B)  ((R)+((G)<<5)+((B)<<10)) 


@ void Paint (unsigned short* dst, unsigned char* sprite, int stride_pixels, unsigned int color)
@ r0 - dst
@ r1 - sprite
@ r2 - stride_pixels
@ r3 - color

.globl Paint
Paint:

    stmdb   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    mov r10,#0x1f

    and r4,r3,r10           @ int ball_r = color & 0x1f
    and r5,r10,r3,lsr #5    @ int ball_g = (color >> 5) & 0x1f
    and r6,r10,r3,lsr #10   @ int ball_b = (color >> 10) & 0x1f


    mov r7,#0               @ y=0

Continue_Y_Loop:
 
    cmp r7,#16              @ y<side
    bge End_Paint
    
    mov r8,#0               @ x=0

Continue_X_Loop:

    cmp r8,#16              @ x<side
    bge End_X_Loop

    ldrb r9,[r1]            @ int t = *sprite++
    add r1,r1,#1

    cmp r9,#0               @ if (t)
    beq T_False

    ldrh r9,[r0]            @ unsigned int back_color = *dst

    @orr r12,r4,r5,lsl #5
    
    @orr r12,r12,r6,lsl #10
    
    and r11,r10,r9          @ int r = back_color & 0x1f
    
    add r11,r11,r11,lsl #1  @ r = ((r << 1) + r
    add r11,r11,r4          @ r = ((r << 1) + r + ball_r)
    mov r11,r11,asr #2      @ r = ((r << 1) + r + ball_r) >> 2
    
    and r12,r10,r9,asr #5  @ int g = (back_color >> 5) & 0x1f
    add r12,r12,r12,lsl #1 @ g = ((g << 1) + g
    add r12,r12,r5         @ g = ((g << 1) + g + ball_g)
    mov r12,r12,asr #2     @ g = ((g << 1) + g + ball_g) >> 2

    orr r11,r11,r12,lsl #5 @ *dst = r | (g << 5)

    and r12,r10,r9,asr #10 @ int b = (back_color >> 10) & 0x1f
    add r12,r12,r12,lsl #1 @ b = ((b << 1) + b
    add r12,r12,r6         @ b = ((b << 1) + b + ball_b)
    mov r12,r12,asr #2     @ b = ((b << 1) + b + ball_b) >> 2
    
    orr r11,r11,r12,lsl #10    @ *dst = r | (g << 5) | (b<<10)

    strh r11,[r0]
T_False:
    add r0,r0,#2            @ dst++
    add r8,r8,#1            @ x++
    b Continue_X_Loop

End_X_Loop:

    @ dst += stride_pixels - side;
    sub r3,r2,#16
    add r0,r0,r3,lsl #1

    add r7,r7,#1            @ y++

    b Continue_Y_Loop
End_Paint:
    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    
    bx lr


