
void __regparm3 FUN_00042232(short *param_1,undefined4 param_2,int param_3)

{
  short sVar1;
  undefined4 uVar2;
  
  if (param_3 < 1) {
    if (*param_1 == 0x7fff) {
      FUN_000426a0((int)*(short *)((int)param_1 + 0x17),1);
      return;
    }
    uVar2 = 1;
    sVar1 = *(short *)((int)param_1 + 0x17);
  }
  else if (*param_1 == 0x7fff) {
    uVar2 = 0;
    sVar1 = *(short *)((int)param_1 + 0x17);
  }
  else {
    uVar2 = 0;
    sVar1 = *(short *)((int)param_1 + 0x17);
  }
  FUN_000426a0((int)sVar1,uVar2);
  return;
}

