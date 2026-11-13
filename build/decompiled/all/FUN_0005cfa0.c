
void FUN_0005cfa0(void)

{
  int *piVar1;
  byte *unaff_EBX;
  undefined8 uVar2;
  
  if ((*unaff_EBX & 1) != 0) {
    uVar2 = FUN_0005cf60();
    piVar1 = (int *)((ulonglong)uVar2 >> 0x20);
    if ((int)uVar2 == 0) {
      unaff_EBX[0] = 0;
      unaff_EBX[1] = 0;
      unaff_EBX[2] = 0;
      unaff_EBX[3] = 0;
      *piVar1 = *piVar1 + 5;
    }
  }
  return;
}

