
void __regparm3 FUN_0005f330(int param_1)

{
  int *piVar1;
  int *extraout_ECX;
  int iVar2;
  int extraout_EDX;
  int iVar3;
  
  piVar1 = (int *)(param_1 + 0x42);
  iVar3 = 0;
  iVar2 = 0;
  if (*(int *)(param_1 + 0x4a) < 1) {
    return;
  }
  do {
    iVar3 = iVar3 + 1;
    (**(code **)(*(int *)(*(int *)(iVar2 + *piVar1) + 0x10) + 4))();
    piVar1 = extraout_ECX;
    iVar2 = extraout_EDX;
  } while (iVar3 < extraout_ECX[2]);
  return;
}

