
void __regparm3 FUN_000266b0(int param_1)

{
  int iVar1;
  char extraout_CL;
  undefined1 extraout_DH;
  uint auStack_24 [3];
  undefined1 local_18;
  
  if ((*(byte *)(param_1 + 99) & 0x20) != 0) {
    iVar1 = (**(code **)(*(int *)(*(int *)(param_1 + 0x124) + 8) + 0xdc))();
    FUN_00026670();
    local_18 = extraout_DH;
    FUN_000749a0();
    auStack_24[(uint)(int)extraout_CL >> 5] =
         auStack_24[(uint)(int)extraout_CL >> 5] | 1 << (extraout_CL % ' ' & 0x1fU);
    FUN_00049580();
    (**(code **)(*(int *)(iVar1 + 8) + 4))();
    (**(code **)(*(int *)(param_1 + 8) + 0x80))();
  }
  FUN_0003d2c0();
  return;
}

