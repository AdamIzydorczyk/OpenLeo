
void __regparm3 FUN_00052540(int param_1,undefined4 param_2,undefined4 param_3)

{
  *(undefined4 *)(param_1 + 0x5c) = 0;
  *(undefined4 *)(param_1 + 0x54) = 0;
  *(undefined4 *)(param_1 + 0x4c) = 0;
  *(undefined4 *)(param_1 + 0x58) = *(undefined4 *)(param_1 + 0x5c);
  *(undefined4 *)(param_1 + 0x50) = *(undefined4 *)(param_1 + 0x54);
  *(undefined4 *)(param_1 + 0x48) = *(undefined4 *)(param_1 + 0x4c);
  FUN_00051eb0(param_3);
  return;
}

