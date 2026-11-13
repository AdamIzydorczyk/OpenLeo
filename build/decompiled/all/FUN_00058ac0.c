
void __regparm3 FUN_00058ac0(int param_1)

{
  int extraout_EDX;
  int iVar1;
  
  FUN_00058b00();
  iVar1 = (extraout_EDX - *(int *)(param_1 + 8)) + 1;
  *(int *)(param_1 + 4) = iVar1;
  if (iVar1 < 0) {
    *(undefined4 *)(param_1 + 4) = 0;
  }
  FUN_00059b70();
  return;
}

