
@ int Biggest (int a, int b);
@ r0 - a
@ r1 - b
@ return in r0

.global Biggest @ hacemos la etiqueta publica

Biggest:
    cmp r1,r0       @ comparar b con a
    ble is_bigger   @
    mov r0,r1       @ a = b
is_bigger:
    bx lr @ antes haciamos "mov pc,lr"

@int Biggest (int a, int b)
@{
@  int res = a;
@  if (b > a)
@    res = b;

@  return res;
@}



@ int Smallest (int a, int b);
@ r0 - a
@ r1 - b
@ return in r0

.global Smallest @ hacemos la etiqueta publica

Smallest:
    cmp r1,r0       @ comparar b con a
    bge is_smaller  @
    mov r0,r1       @ res = b
is_smaller:
    bx lr @ antes haciamos "mov pc,lr"

@int Smallest (int a, int b)
@{
@  int res = a;
@  if (b < a)
@    res = b;

@  return res;
@}

