
int __regparm3 FUN_0004b7f0(int param_1,int param_2)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar2;
  
  iVar2 = 0;
  if (0 < *(int *)(param_1 + 0x84)) {
    iVar1 = 0;
    do {
      iVar1 = (**(code **)(*(int *)(*(int *)(param_1 + 0x78) + iVar1 + 0x10) + 4))();
      if (iVar1 != 0) {
        *(undefined1 *)(param_2 + 0xc) = 0;
        FUN_000749a0();
        *(byte *)(extraout_ECX_00 + 3) = *(byte *)(extraout_ECX_00 + 3) | 2;
        return param_2;
      }
      iVar2 = iVar2 + 1;
      iVar1 = extraout_ECX + 0x14;
    } while (iVar2 < *(int *)(param_1 + 0x84));
  }
  *(undefined1 *)(param_2 + 0xc) = 0;
  FUN_000749a0();
  return param_2;
}

