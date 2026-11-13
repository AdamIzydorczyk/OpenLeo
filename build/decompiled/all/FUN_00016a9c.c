
bool FUN_00016a9c(void)

{
  int iVar1;
  int extraout_ECX;
  int unaff_EBX;
  bool bVar2;
  
  if (*(int *)(unaff_EBX + 1) != 0) {
    (**(code **)(**(int **)(unaff_EBX + 1) + 0x1c))();
    unaff_EBX = extraout_ECX;
  }
  if (*(int **)(unaff_EBX + 1) != (int *)0x0) {
    iVar1 = (**(code **)(**(int **)(unaff_EBX + 1) + 0x18))();
    bVar2 = iVar1 == -1;
    if (!bVar2) goto LAB_00016ad5;
  }
  bVar2 = true;
LAB_00016ad5:
  return !bVar2;
}

