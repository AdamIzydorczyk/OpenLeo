
int __regparm3 FUN_0004b320(int param_1)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  uint uVar5;
  int *extraout_ECX;
  int extraout_ECX_00;
  uint uVar6;
  uint uVar7;
  
  piVar3 = (int *)(**(code **)(*(int *)(*(int *)(param_1 + 0x394) + 8) + 0x68))();
  iVar1 = *extraout_ECX;
  iVar2 = *piVar3;
  uVar6 = iVar1 - iVar2 >> 0x1f;
  iVar4 = (**(code **)(*(int *)(*(int *)(param_1 + 0x394) + 8) + 0x68))();
  uVar5 = *(int *)(extraout_ECX_00 + 4) - *(int *)(iVar4 + 4);
  uVar7 = (int)uVar5 >> 0x1f;
  return ((uVar5 ^ uVar7) - uVar7) + ((iVar1 - iVar2 ^ uVar6) - uVar6);
}

