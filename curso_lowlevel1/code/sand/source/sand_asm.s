

@typedef struct {       sizeof() == 4
@  unsigned char  x, y;     offs 0,
@  unsigned char  alive;         2
@  char           PADDING; // Just to be sure sizeof(Particle) is a factor of 4
@} Particle;

@typedef struct         sizeof() == 4
@{
@  unsigned short x, y;     offs 0
@} Source;


@ void SeedParticles (const Source* sources, Particle* particles, int nparticles, unsigned short* screen)
@ r0 - source
@ r1 - particles
@ r2 - nparticles
@ r3 - screen

.globl SeedParticles

SeedParticles:

    stmdb   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}

    mov     r8,#8       @ int nsources = NUM_SOURCES;
seed_loop:
    ldrb    r4,[r1,#2]
    cmp     r4,#0       @ if (!particles->alive)
    bne     seed_next_particle

    ldrh    r4,[r0,#0]  @ int x = sources->x;
    ldrh    r5,[r0,#2]  @ int y = sources->y;

    @ unsigned short pixel = screen[x + y * SCREEN_W];

    mov     r7,#240
    mul     r6,r5,r7
    add     r6,r6,r4    @ x + y * SCREEN_W
    add     r6,r6,r6
    ldrh    r7,[r3,r6]
    
    mov     r6,#1
    cmp     r7,#0       @ if (pixel == BLACK)
    streqb  r4,[r1,#0]  @ particles->x = x;
    streqb  r5,[r1,#1]  @ particles->y = y;
    streqb  r6,[r1,#2]  @ particles->alive = 1;
    
    add     r0,r0,#4    @ sources++;
    sub     r8,r8,#1    @ nsources--;

seed_next_particle:

    add     r1,r1,#4    @ particles++
    sub     r2,r2,#1    @ nparciles--
    
    @ while ((nparticles != 0) && (nsources != 0));
    
    cmp     r2,#0
    beq     seed_done
    cmp     r8,#0
    bne     seed_loop

seed_done:
    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    
    bx lr

@typedef struct {       sizeof() == 4
@  unsigned char  x, y;     offs 0,
@  unsigned char  alive;         2
@  char           PADDING; // Just to be sure sizeof(Particle) is a factor of 4
@} Particle;

@typedef struct         sizeof() == 4
@{
@  unsigned short x, y;     offs 0
@} Source;

@ void UpdateParticles (Particle* particles, int nparticles, unsigned short* screen)
@ r0 - particles
@ r1 - nparticles
@ r2 - screen

.globl UpdateParticles

UpdateParticles:
    stmdb   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}

update_loop:
    ldrb    r3,[r0,#2]
    cmp     r3,#0               @ if (particles->alive)
    beq     update_next_particle
    
    ldrb    r3,[r0,#0]          @ int x = particles->x;
    ldrb    r4,[r0,#1]          @ int y = particles->y;

    @ unsigned short* current = screen + x + SCREEN_W * y;
    
    mov     r5,#240             @ SCREEN_W
    mul     r6,r4,r5            @ y * SCREEN_W
    add     r6,r6,r3            @ x + y * SCREEN_W
    add     r6,r6,r6            @ size(unsigned short) = 2
    add     r5,r2,r6            @ current

    @ unsigned short* down = current + SCREEN_W;
    
    add     r6,r5,#(240 * 2)    @ down = current + SCREEN_W
    
    mov     r7,#66              @ int new_x = 66;
    
    ldrh    r8,[r6,#0]          @ if (down[0] == BLACK)
    cmp     r8,#0
    bne     update_0_not_black

    mov     r7,#0               @ new_x = 0;
    b       update_end_ifs
  
update_0_not_black:

    ldrh    r8,[r6,#-2]  @ if (down[-1] == BLACK)
    cmp     r8,#0
    bne     update_minus1_not_black

    mov     r7,#-1       @ new_x = -1;

    b       update_end_ifs

update_minus1_not_black:

    ldrh    r8,[r6,#2]  @ if (down[1] == BLACK)
    cmp     r8,#0
    bne     update_end_ifs

    mov     r7,#1       @ new_x = 1;

update_end_ifs:
    
    cmp     r7,#66              @ if (new_x != 66)
    beq     update_else
    
    mov     r8,#0
    strh    r8,[r5]             @ *current = BLACK (0);

    mov     r8,#-1
    add     r9,r7,r7
    strh    r8,[r6,r9]             @ down[new_x] = WHITE (0xffff);
    
    add     r8,r3,r7            @ x + new_x
    strb    r8,[r0,#0]          @ particles->x = x + new_x;

    add     r8,r4,#1            @ y + 1
    strb    r8,[r0,#1]          @ particles->y = y + 1;

    b       update_next_particle

update_else:
    mov     r8,#0
    strb    r8,[r0,#2]          @ particles->alive = 0;


update_next_particle:

    add r0,r0,#4                @ particles++;
    sub r1,r1,#1                @ nparticles--;
    
    cmp     r1,#0
    beq     seed_done
    b       update_loop

update_done:

    ldmia   sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
    
    bx lr













