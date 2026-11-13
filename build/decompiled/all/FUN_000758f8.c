
void __regparm3 FUN_000758f8(int param_1,undefined4 param_2,undefined4 param_3)

{
  int extraout_ECX;
  uint unaff_EBX;
  undefined8 uVar1;
  
  *(undefined4 **)(param_1 + 8) = &DAT_000cf4f4;
  *(undefined4 *)(param_1 + 0x18) = 0;
  *(undefined4 *)(param_1 + 4) = param_2;
  *(byte *)(param_1 + 0x24) = *(byte *)(param_1 + 0x24) & 0xfe;
  *(uint *)(param_1 + 0x24) = *(uint *)(param_1 + 0x24) | unaff_EBX & 1;
  uVar1 = FUN_00075874(param_3);
  *(int *)(extraout_ECX + 0xc) = (int)uVar1;
  if ((int)((ulonglong)uVar1 >> 0x20) == 0) {
    *(byte *)(extraout_ECX + 0x24) = *(byte *)(extraout_ECX + 0x24) | 2;
    return;
  }
  *(byte *)(extraout_ECX + 0x24) = *(byte *)(extraout_ECX + 0x24) & 0xfd;
  return;
}

