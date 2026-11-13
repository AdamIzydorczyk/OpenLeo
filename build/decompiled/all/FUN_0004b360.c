
void __regparm3 FUN_0004b360(int param_1,int param_2)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar2;
  int extraout_EDX;
  undefined8 uVar3;
  
  iVar1 = param_1 + 0xc;
  do {
    if (param_2 < *(int *)(param_1 + 0x44)) {
      for (iVar2 = *(int *)(param_2 * 4 + *(int *)(param_1 + 0x58)); iVar2 != 0;
          iVar2 = *(int *)(iVar2 + 0x34)) {
        uVar3 = (**(code **)(*(int *)(iVar2 + 8) + 0x60))();
        iVar2 = (int)((ulonglong)uVar3 >> 0x20);
        param_1 = extraout_ECX;
        if ((int)uVar3 != 0) {
          (**(code **)(*(int *)(iVar2 + 8) + 0x74))();
          param_1 = extraout_ECX_00;
          iVar2 = extraout_EDX;
        }
      }
    }
    param_1 = param_1 + 4;
  } while (param_1 != iVar1);
  return;
}

