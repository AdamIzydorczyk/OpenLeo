
int __regparm3 FUN_0003fd00(int param_1,int param_2)

{
  int iVar1;
  int extraout_ECX;
  int unaff_EBX;
  
  if (*(int *)(*(int *)(param_1 + 0x2a) + unaff_EBX * 8 + param_2 * 4) == 0) {
    iVar1 = (**(code **)(*(int *)(param_1 + 0x2e) + 0x18))();
    *(int *)(*(int *)(extraout_ECX + 0x2a) + unaff_EBX * 8 + param_2 * 4) = iVar1 + 1;
    param_1 = extraout_ECX;
  }
  return *(int *)(*(int *)(param_1 + 0x2a) + unaff_EBX * 8 + param_2 * 4) + -1;
}

