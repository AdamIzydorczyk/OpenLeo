
void __regparm3 FUN_0001f490(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  int extraout_EDX;
  
  iVar1 = *(int *)(DAT_000cef40 + 0x1c);
  *(int *)(DAT_000cef40 + 0x1c) = 0;
  if (iVar1 != 0) {
    FUN_0001f330(param_3);
    param_2 = extraout_EDX;
  }
  iVar1 = (**(code **)(*(int *)(param_2 + 8) + 0x3c))();
  if (iVar1 != 0) {
    iVar1 = FUN_0001f590();
    if (iVar1 != 0) {
      *(byte *)(param_1 + 0x66) = *(byte *)(param_1 + 0x66) | 0x80;
      goto LAB_0001f4d6;
    }
  }
  *(byte *)(param_1 + 0x66) = *(byte *)(param_1 + 0x66) | 0x40;
LAB_0001f4d6:
  FUN_0003df50();
  return;
}

