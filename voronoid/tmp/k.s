
build/main.o:     file format elf32-littlearm


Disassembly of section .text:

00000000 <Voronoid>:
   0:	e92d47f0 	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   4:	e1a09000 	mov	r9, r0
   8:	e1a08001 	mov	r8, r1
   c:	e1a0a002 	mov	r10, r2
  10:	e1a07003 	mov	r7, r3
  14:	e3a06000 	mov	r6, #0
  18:	e1a05009 	mov	r5, r9
  1c:	e3a04000 	mov	r4, #0
  20:	e1a03006 	mov	r3, r6
  24:	e1a02004 	mov	r2, r4
  28:	e1a0100a 	mov	r1, r10
  2c:	e1a00008 	mov	r0, r8
  30:	ebfffffe 	bl	0 <Closest>
  34:	e1a00080 	lsl	r0, r0, #1
  38:	e19730b0 	ldrh	r3, [r7, r0]
  3c:	e2844001 	add	r4, r4, #1
  40:	e35400f0 	cmp	r4, #240	; 0xf0
  44:	e0c530b2 	strh	r3, [r5], #2
  48:	1afffff4 	bne	20 <Voronoid+0x20>
  4c:	e2866001 	add	r6, r6, #1
  50:	e35600a0 	cmp	r6, #160	; 0xa0
  54:	e2899e1e 	add	r9, r9, #480	; 0x1e0
  58:	1affffee 	bne	18 <Voronoid+0x18>
  5c:	e8bd47f0 	pop	{r4, r5, r6, r7, r8, r9, r10, lr}
  60:	e12fff1e 	bx	lr

Disassembly of section .text.startup:

00000000 <main>:
   0:	e92d4010 	push	{r4, lr}
   4:	ebfffffe 	bl	0 <irqInit>
   8:	e3a00001 	mov	r0, #1
   c:	ebfffffe 	bl	0 <irqEnable>
  10:	e3a00001 	mov	r0, #1
  14:	e3a03301 	mov	r3, #67108864	; 0x4000000
  18:	e59f1028 	ldr	r1, [pc, #40]	; 48 <main+0x48>
  1c:	e59f2028 	ldr	r2, [pc, #40]	; 4c <main+0x4c>
  20:	e1c100b8 	strh	r0, [r1, #8]
  24:	e59f4024 	ldr	r4, [pc, #36]	; 50 <main+0x50>
  28:	e5832000 	str	r2, [r3]
  2c:	ef050000 	svc	0x00050000
  30:	e3a02006 	mov	r2, #6
  34:	e1a03004 	mov	r3, r4
  38:	e3a00406 	mov	r0, #100663296	; 0x6000000
  3c:	e59f1010 	ldr	r1, [pc, #16]	; 54 <main+0x54>
  40:	ebfffffe 	bl	0 <main>
  44:	eafffff8 	b	2c <main+0x2c>
  48:	04000200 	.word	0x04000200
  4c:	00000403 	.word	0x00000403
  50:	00000000 	.word	0x00000000
  54:	0000000c 	.word	0x0000000c
