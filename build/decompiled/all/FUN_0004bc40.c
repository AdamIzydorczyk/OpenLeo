
void __regparm3 FUN_0004bc40(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int extraout_ECX;
  int local_24;
  int local_18;
  int local_14;
  
  local_24 = 0;
  do {
    local_14 = 0;
    for (local_18 = 0; local_18 < *(int *)(param_1 + local_24 + 0x44); local_18 = local_18 + 1) {
      for (iVar1 = *(int *)(*(int *)(param_1 + local_24 + 0x58) + local_14); iVar1 != 0;
          iVar1 = *(int *)(iVar1 + 0x34)) {
        if ((param_2 == 0) ||
           (iVar2 = (**(code **)(*(int *)(iVar1 + 8) + 0xc))(), iVar2 == extraout_ECX)) {
          FUN_00052b10();
        }
      }
      local_14 = local_14 + 4;
    }
    local_24 = local_24 + 4;
  } while (local_24 != 0xc);
  for (iVar1 = *(int *)(param_1 + 100); iVar1 != 0; iVar1 = *(int *)(iVar1 + 0x34)) {
    if ((param_2 == 0) || (iVar2 = (**(code **)(*(int *)(iVar1 + 8) + 0xc))(), iVar2 == param_2)) {
      FUN_00052b10();
    }
  }
  return;
}

