
void __regparm3 FUN_0004bb30(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int local_20;
  int local_14;
  int local_10;
  
  local_20 = 0;
  do {
    local_10 = 0;
    for (local_14 = 0; local_14 < *(int *)(param_1 + local_20 + 0x44); local_14 = local_14 + 1) {
      for (iVar1 = *(int *)(*(int *)(param_1 + local_20 + 0x58) + local_10); iVar1 != 0;
          iVar1 = *(int *)(iVar1 + 0x34)) {
        if ((param_2 == 0) || (iVar2 = (**(code **)(*(int *)(iVar1 + 8) + 0xc))(), iVar2 == param_2)
           ) {
          (**(code **)(*(int *)(iVar1 + 8) + 0x10))();
        }
      }
      local_10 = local_10 + 4;
    }
    local_20 = local_20 + 4;
  } while (local_20 != 0xc);
  for (iVar1 = *(int *)(param_1 + 100); iVar1 != 0; iVar1 = *(int *)(iVar1 + 0x34)) {
    if ((param_2 == 0) || (iVar2 = (**(code **)(*(int *)(iVar1 + 8) + 0xc))(), iVar2 == param_2)) {
      (**(code **)(*(int *)(iVar1 + 8) + 0x10))(0);
    }
  }
  return;
}

