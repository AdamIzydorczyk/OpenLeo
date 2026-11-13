
void __regparm3 FUN_00060320(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int unaff_EBX;
  int local_18;
  int local_10;
  
  if (0 < unaff_EBX) {
    local_18 = 0;
    iVar3 = 0;
    local_10 = 0;
    iVar2 = 0;
    while( true ) {
      for (; iVar3 < *(int *)(param_1 + 0x34); iVar3 = iVar3 + 1) {
        if ((*(int *)(param_2 + local_10) != 0) &&
           (iVar1 = *(int *)(*(int *)(param_1 + 0x2c) + iVar2),
           *(int *)(param_2 + local_10) == *(int *)(iVar1 + 0x28))) {
          *(undefined4 *)(iVar1 + 0x2c) = 2;
        }
        iVar2 = iVar2 + 4;
      }
      local_18 = local_18 + 4;
      if (unaff_EBX * 4 <= local_18) break;
      iVar3 = 0;
      iVar2 = 0;
      local_10 = local_18;
    }
  }
  return;
}

