
.globl SortArray

@ static void SortArray (signed char *array, int len)
@ signed char *array - r0
@ int len - r1

SortArray:
  stmdb   sp!,{r4,r5,r6,r7}     @ prolog

while_loop:
  cmp   r1,#1     
  ble   sort_done   @ while (len < 1)

  mov   r2,#1     @ ordered = 1    
  mov   r3,#0     @ i
  sub   r7,r1,#1  @ len - 1
for_loop:
  cmp   r3,r7
  bge   for_done

  ldrsb r4,[r0,r3]    @ t0 = array[i]
  add   r6,r3,#1      @ i + 1
  ldrsb r5,[r0,r6]    @ t1 = array[i+1]

  cmp   r4,r5
  blt   skip_swap

  strb  r5,[r0,r3]    @ array[i] = t1
  strb  r4,[r0,r6]    @ array[i+1] = t0
  mov   r2,#0         @ ordered = 0

skip_swap:
  add   r3,r3,#1   @ i++
  b     for_loop

for_done:
  cmp   r2,#0
  bne   sort_done   @ if (ordered) return
  sub   r1,r1,#1    @ len--
  b     while_loop

sort_done:
  ldmia   sp!,{r4,r5,r6,r7}    @ epilog
  bx      lr

