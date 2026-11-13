
bool __regparm3 FUN_00016ae4(undefined4 param_1,int param_2,undefined4 param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined1 *extraout_ECX;
  int extraout_EDX;
  int unaff_EBX;
  
  while (0 < param_2) {
    uVar1 = FUN_00016a18();
    *extraout_ECX = uVar1;
    param_2 = extraout_EDX + -1;
  }
  if (*(int **)(unaff_EBX + 1) != (int *)0x0) {
    iVar2 = (**(code **)(**(int **)(unaff_EBX + 1) + 0x18))(param_3);
    if (iVar2 != -1) {
      return iVar2 == -1;
    }
  }
  return true;
}

