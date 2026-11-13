
void __regparm3 FUN_00045400(int param_1,int param_2,int param_3,int param_4)

{
  int iVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_EDX;
  int iVar3;
  
  if ((int)(uint)*(byte *)(*(int *)(param_1 + 4) + 0xc) <= param_3) {
    iVar1 = FUN_00045380();
    if (iVar1 != 0) {
      (**(code **)(*(int *)(param_2 + 8) + 0x6c))();
      iVar1 = *(int *)(param_1 + 4);
      iVar3 = iVar1 + (extraout_EDX + -1) * 0xd;
      iVar2 = FUN_000455a0(iVar1,iVar3);
      if ((param_1 == *(int *)(param_2 + 0xa9)) || (-1 < iVar2 - param_4)) {
        iVar1 = FUN_000455a0(iVar1,iVar3);
        if ((iVar1 < 1) && (iVar1 <= *(int *)(param_2 + 0xea))) {
          *(int *)(param_2 + 0xe6) = param_1;
          *(int *)(param_2 + 0xea) = iVar1;
          *(uint *)(param_2 + 0xe2) = (uint)*(byte *)(extraout_ECX + 0xc + *(int *)(param_1 + 4));
        }
      }
    }
  }
  return;
}

