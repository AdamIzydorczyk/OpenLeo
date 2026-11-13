
int __regparm3 FUN_000287a0(undefined4 param_1,int param_2)

{
  int iVar1;
  int extraout_ECX;
  int unaff_EBX;
  
  iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
  if (iVar1 != 0) {
    iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x6c))();
    iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
    if ((iVar1 != 0) &&
       (iVar1 = FUN_00055760(),
       *(int *)(*(int *)(*(int *)(iVar1 + 0x50) + *(int *)(iVar1 + 0x5c) * 4) + 4) == 6)) {
      *(undefined1 *)(param_2 + 0xc) = 0;
      FUN_000749a0();
      *(byte *)(extraout_ECX + 1) = *(byte *)(extraout_ECX + 1) | 4;
      return param_2;
    }
    if ((*(int *)(unaff_EBX + 0x5c) == 0x12) &&
       (*(int *)(*(int *)(*(int *)(unaff_EBX + 0x50) + 0x48) + 4) < 0x1a)) {
      iVar1 = FUN_00055760();
      (**(code **)(*(int *)(iVar1 + 8) + 0x10))(0,0x10,0x8138764,10);
    }
  }
  *(undefined1 *)(param_2 + 0xc) = 0;
  FUN_000749a0();
  return param_2;
}

