
void __regparm3 FUN_00053480(int param_1)

{
  int iVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar3;
  
  iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0x84))();
  iVar2 = FUN_00053440();
  iVar3 = extraout_ECX;
  if (iVar1 == 0) {
    iVar1 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x88))();
    iVar3 = extraout_ECX_00;
  }
  *(int *)(*(int *)(iVar3 + 0x50) + iVar2 * 4) = iVar1;
  return;
}

