
build/closest.o:     file format elf32-littlearm


Disassembly of section .text:

00000000 <Closest>:
   0:	e92d41f0 	push	{r4, r5, r6, r7, r8, lr}
   4:	e1a06000 	mov	r6, r0
   8:	e5905004 	ldr	r5, [r0, #4]
   c:	e0455003 	sub	r5, r5, r3
  10:	e0000595 	mul	r0, r5, r5
  14:	e5965000 	ldr	r5, [r6]
  18:	e0455002 	sub	r5, r5, r2
  1c:	e1a0c005 	mov	r12, r5
  20:	e3510001 	cmp	r1, #1
  24:	e02c0c95 	mla	r12, r5, r12, r0
  28:	da000012 	ble	78 <Closest+0x78>
  2c:	e1a0500c 	mov	r5, r12
  30:	e3a00000 	mov	r0, #0
  34:	e3a04001 	mov	r4, #1
  38:	e2867004 	add	r7, r6, #4
  3c:	e797c184 	ldr	r12, [r7, r4, lsl #3]
  40:	e04cc003 	sub	r12, r12, r3
  44:	e00e0c9c 	mul	lr, r12, r12
  48:	e796c184 	ldr	r12, [r6, r4, lsl #3]
  4c:	e04cc002 	sub	r12, r12, r2
  50:	e1a0800c 	mov	r8, r12
  54:	e028e89c 	mla	r8, r12, r8, lr
  58:	e1580005 	cmp	r8, r5
  5c:	b1a00004 	movlt	r0, r4
  60:	e2844001 	add	r4, r4, #1
  64:	b1a05008 	movlt	r5, r8
  68:	e1510004 	cmp	r1, r4
  6c:	1afffff2 	bne	3c <Closest+0x3c>
  70:	e8bd41f0 	pop	{r4, r5, r6, r7, r8, lr}
  74:	e12fff1e 	bx	lr
  78:	e3a00000 	mov	r0, #0
  7c:	e8bd41f0 	pop	{r4, r5, r6, r7, r8, lr}
  80:	e12fff1e 	bx	lr
