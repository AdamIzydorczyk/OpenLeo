
int __regparm3 FUN_0004b6d0(int param_1,int param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int extraout_ECX;
  int *unaff_EBX;
  int *piVar4;
  int iVar5;
  int local_14;
  
  piVar1 = (int *)*unaff_EBX;
  iVar5 = 0;
  local_14 = -0x7fffffff;
  if (0 < *(int *)(param_1 + 0x80)) {
    iVar3 = 0;
    do {
      piVar4 = (int *)(*(int *)(param_1 + 0x74) + iVar3);
      if (piVar4 != piVar1) {
        iVar3 = *piVar4;
        (**(code **)(*(int *)(param_2 + 8) + 0x6c))();
        iVar2 = (**(code **)(iVar3 + 0x14))();
        iVar3 = extraout_ECX;
        if ((iVar2 < 0) && (local_14 < iVar2)) {
          *unaff_EBX = *(int *)(param_1 + 0x74) + extraout_ECX;
          local_14 = iVar2;
        }
      }
      iVar5 = iVar5 + 1;
      iVar3 = iVar3 + 0xc;
    } while (iVar5 < *(int *)(param_1 + 0x80));
  }
  return local_14;
}

