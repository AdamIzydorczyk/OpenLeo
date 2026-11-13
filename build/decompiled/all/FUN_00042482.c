
void __regparm3
FUN_00042482(int param_1,undefined4 param_2,int param_3,undefined4 param_4,undefined4 param_5,
            char param_6)

{
  short sVar1;
  undefined4 uVar2;
  
  if (param_3 < 1) {
    if (*(short *)(param_1 + 4) == 0x7fff) {
      FUN_000428c0((int)*(short *)(param_1 + 0x17),1,(int)param_6);
      return;
    }
    uVar2 = 1;
    sVar1 = *(short *)(param_1 + 0x17);
  }
  else if (*(short *)(param_1 + 4) == 0x7fff) {
    uVar2 = 0;
    sVar1 = *(short *)(param_1 + 0x17);
  }
  else {
    uVar2 = 0;
    sVar1 = *(short *)(param_1 + 0x17);
  }
  FUN_000428c0((int)sVar1,uVar2,(int)param_6);
  return;
}

