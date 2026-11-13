
void __regparm3 FUN_00052070(int *param_1,undefined4 param_2,undefined4 param_3)

{
  short sVar1;
  undefined8 uVar2;
  
  *(undefined4 *)(*param_1 + 0x9d) = 0;
  if (*(short *)((int)param_1 + 10) == 0) {
    sVar1 = (short)param_1[2];
  }
  else {
    uVar2 = FUN_00076bcc(param_2,param_3);
    sVar1 = (short)((uint)((int)uVar2 * (int)((ulonglong)uVar2 >> 0x20)) / 0x7fff) +
            (short)param_1[2];
  }
  *(short *)(param_1 + 3) = sVar1;
  return;
}

