
void __regparm3 FUN_00044e90(int *param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int unaff_EBX;
  int iVar4;
  
  iVar3 = 0;
  if (0 < param_1[1]) {
    iVar4 = 0;
    do {
      iVar2 = *param_1;
      piVar1 = (int *)(iVar4 + iVar2);
      *piVar1 = *piVar1 + param_2;
      piVar1 = (int *)(iVar4 + 4 + iVar2);
      *piVar1 = *piVar1 + unaff_EBX;
      iVar3 = iVar3 + 1;
      iVar4 = iVar4 + 0xc;
    } while (iVar3 < param_1[1]);
  }
  return;
}

