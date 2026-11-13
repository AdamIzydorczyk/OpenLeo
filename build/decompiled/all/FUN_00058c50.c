
undefined4 __regparm3 FUN_00058c50(int param_1,undefined4 *param_2)

{
  bool bVar1;
  int iVar2;
  undefined4 extraout_ECX;
  int local_14;
  
  if (((char *)*param_2 == (char *)0x0) || (*(char *)*param_2 == '\0')) {
    bVar1 = true;
  }
  else {
    bVar1 = false;
  }
  if (!bVar1) {
    local_14 = FUN_00058bf0();
    while (iVar2 = FUN_00058a70(), *(int *)(iVar2 + 0xe) != 0) {
      iVar2 = FUN_00077010();
      if (iVar2 == 0) {
        return extraout_ECX;
      }
      if (local_14 == 0) {
        local_14 = *(int *)(param_1 + 0x10);
      }
      local_14 = local_14 + -1;
    }
  }
  return 0;
}

