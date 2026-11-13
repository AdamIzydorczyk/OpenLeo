
void FUN_0005ba70(void)

{
  int *piVar1;
  undefined8 uVar2;
  
  uVar2 = FUN_0005bac0();
  piVar1 = (int *)((ulonglong)uVar2 >> 0x20);
  if ((int)uVar2 != 0) {
    if (*piVar1 == 0) {
      DAT_000cf1bc = piVar1[1];
    }
    else {
      *(int *)(*piVar1 + 4) = piVar1[1];
    }
    if ((int *)piVar1[1] != (int *)0x0) {
      *(int *)piVar1[1] = *piVar1;
    }
    *piVar1 = 0;
    piVar1[4] = piVar1[4] & 0xfffffffd;
  }
  return;
}

