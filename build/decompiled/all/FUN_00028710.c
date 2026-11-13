
int __regparm3 FUN_00028710(undefined4 param_1,int param_2)

{
  int iVar1;
  int extraout_ECX;
  int unaff_EBX;
  
  iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
  if (iVar1 == 0) {
LAB_00028778:
    *(undefined1 *)(param_2 + 0xc) = 0;
    FUN_000749a0();
  }
  else {
    if ((*(int *)(unaff_EBX + 0x5c) < 9) || (0xd < *(int *)(unaff_EBX + 0x5c))) {
LAB_00028748:
      iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x68))();
      iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
      if (iVar1 == 0) goto LAB_00028778;
    }
    else {
      iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x6c))();
      iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
      if (iVar1 == 0) goto LAB_00028748;
    }
    *(undefined1 *)(param_2 + 0xc) = 0;
    FUN_000749a0();
    *(byte *)(extraout_ECX + 2) = *(byte *)(extraout_ECX + 2) | 2;
  }
  return param_2;
}

