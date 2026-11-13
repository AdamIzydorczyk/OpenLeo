
undefined4 FUN_00075448(void)

{
  int iVar1;
  undefined4 in_ECX;
  int extraout_ECX;
  
  iVar1 = FUN_000753ac(in_ECX);
  if ((iVar1 == 0) || (*(int *)(extraout_ECX + 2 + iVar1 * 4) == 0)) {
    return 1;
  }
  return 0;
}

