
void __regparm3 FUN_00053410(int param_1,int *param_2)

{
  int iVar1;
  int *unaff_EBX;
  
  if (*param_2 == 0) {
    iVar1 = *(int *)(param_1 + 0x5c);
    *param_2 = iVar1;
  }
  else {
    if (*unaff_EBX != -1) {
      return;
    }
    iVar1 = *(int *)(param_1 + 0x5c);
  }
  *unaff_EBX = *(int *)(*(int *)(*(int *)(param_1 + 0x50) + iVar1 * 4) + 4);
  return;
}

