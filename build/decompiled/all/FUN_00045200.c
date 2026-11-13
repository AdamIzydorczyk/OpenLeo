
void __regparm3 FUN_00045200(int param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int unaff_EBX;
  int iVar4;
  
  iVar3 = 0;
  if (-1 < *(int *)(param_1 + 8)) {
    iVar4 = 0;
    do {
      iVar2 = *(int *)(param_1 + 4);
      piVar1 = (int *)(iVar4 + iVar2);
      *piVar1 = *piVar1 + param_2;
      piVar1 = (int *)(iVar4 + 4 + iVar2);
      *piVar1 = *piVar1 + unaff_EBX;
      iVar3 = iVar3 + 1;
      iVar4 = iVar4 + 0xd;
    } while (iVar3 <= *(int *)(param_1 + 8));
  }
  return;
}

