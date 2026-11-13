
undefined4 __regparm3 FUN_0003a000(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int unaff_EBX;
  
  iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
  iVar2 = extraout_ECX;
  if (iVar1 != 0) {
    iVar2 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x68))();
    iVar1 = (**(code **)(*(int *)(iVar2 + 8) + 8))();
    iVar2 = extraout_ECX_00;
    if (iVar1 != 0) {
      *(byte *)(extraout_ECX_00 + 0x66) = *(byte *)(extraout_ECX_00 + 0x66) | 0x40;
      goto LAB_0003a03e;
    }
  }
  *(byte *)(iVar2 + 0x66) = *(byte *)(iVar2 + 0x66) | 0x80;
LAB_0003a03e:
  FUN_000552e0();
  return param_2;
}

