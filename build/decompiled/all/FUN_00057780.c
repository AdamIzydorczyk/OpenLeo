
void __regparm3 FUN_00057780(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  int extraout_ECX;
  
  iVar3 = FUN_00057500();
  iVar4 = FUN_00057500();
  iVar4 = iVar4 + (iVar3 / 0x50) * -0x50;
  iVar5 = FUN_00057500(iVar4);
  iVar6 = FUN_00057500();
  iVar1 = *(int *)(*(int *)(param_1 + 0x44) + extraout_ECX * 4 + 4);
  iVar2 = *(int *)(iVar1 + 8);
  uVar7 = FUN_00057530(iVar4,iVar6 + (iVar5 / 0x50) * -0x50,iVar1);
  FUN_00057530(*(undefined4 *)(*(int *)(param_1 + 0x44) + extraout_ECX * 4),iVar3 / 0x50,iVar4,uVar7
              );
  (**(code **)(iVar2 + 0xc))();
  return;
}

