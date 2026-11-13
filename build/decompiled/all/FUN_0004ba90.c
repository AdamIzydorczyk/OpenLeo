
void __regparm3 FUN_0004ba90(int param_1)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int iVar2;
  int extraout_EDX;
  int iVar3;
  int iVar4;
  undefined8 uVar5;
  int local_1c;
  
  local_1c = 0;
  iVar3 = 0;
  iVar1 = 0;
  iVar4 = 0;
  while( true ) {
    for (; iVar1 < *(int *)(iVar3 + param_1 + 0x44); iVar1 = iVar1 + 1) {
      for (iVar2 = *(int *)(*(int *)(iVar3 + param_1 + 0x58) + iVar4); iVar2 != 0;
          iVar2 = *(int *)(iVar2 + 0x34)) {
        uVar5 = (**(code **)(*(int *)(iVar2 + 8) + 0x60))();
        iVar2 = (int)((ulonglong)uVar5 >> 0x20);
        iVar1 = extraout_ECX;
        if ((int)uVar5 != 0) {
          FUN_00052b80();
          iVar1 = extraout_ECX_00;
          iVar2 = extraout_EDX;
        }
      }
      iVar4 = iVar4 + 4;
    }
    iVar3 = local_1c + 4;
    if (iVar3 == 0xc) break;
    iVar1 = 0;
    iVar4 = 0;
    local_1c = iVar3;
  }
  for (iVar3 = *(int *)(param_1 + 100); iVar3 != 0; iVar3 = *(int *)(iVar3 + 0x34)) {
    iVar1 = (**(code **)(*(int *)(iVar3 + 8) + 0x60))();
    iVar3 = extraout_ECX_01;
    if (iVar1 != 0) {
      FUN_00052b80();
      iVar3 = extraout_ECX_02;
    }
  }
  return;
}

