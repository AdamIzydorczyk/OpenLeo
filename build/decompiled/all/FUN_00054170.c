
void __regparm3 FUN_00054170(int param_1,int param_2)

{
  int iVar1;
  int unaff_EBX;
  
  if (param_2 < 1) {
    if ((*(int *)(param_1 + 0xc2) <= param_2) ||
       (iVar1 = *(int *)(param_1 + 0xc2) + param_2, *(int *)(param_1 + 0xc2) = iVar1,
       param_2 <= iVar1)) goto LAB_000541ae;
  }
  else if ((param_2 <= *(int *)(param_1 + 0xc2)) ||
          (iVar1 = param_2 + *(int *)(param_1 + 0xc2), *(int *)(param_1 + 0xc2) = iVar1,
          iVar1 <= param_2)) goto LAB_000541ae;
  *(int *)(param_1 + 0xc2) = param_2;
LAB_000541ae:
  if (unaff_EBX < 1) {
    if (*(int *)(param_1 + 0xc6) <= unaff_EBX) {
      return;
    }
    iVar1 = *(int *)(param_1 + 0xc6) + unaff_EBX;
    *(int *)(param_1 + 0xc6) = iVar1;
    if (unaff_EBX <= iVar1) {
      return;
    }
  }
  else {
    if (unaff_EBX <= *(int *)(param_1 + 0xc6)) {
      return;
    }
    iVar1 = *(int *)(param_1 + 0xc6) + unaff_EBX;
    *(int *)(param_1 + 0xc6) = iVar1;
    if (iVar1 <= unaff_EBX) {
      return;
    }
  }
  *(int *)(param_1 + 0xc6) = unaff_EBX;
  return;
}

