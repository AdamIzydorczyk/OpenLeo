
bool __regparm3 FUN_00075485(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int extraout_ECX;
  
  iVar1 = FUN_000753ac();
  if (iVar1 != 0) {
    *(undefined4 *)(extraout_ECX + 0x18) = param_2;
    *(int *)(extraout_ECX + 0x20) = iVar1 + -1;
  }
  return iVar1 != 0;
}

