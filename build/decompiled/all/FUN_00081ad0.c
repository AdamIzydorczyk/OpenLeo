
void __regparm3 FUN_00081ad0(int param_1,uint param_2,int param_3)

{
  uint *puVar1;
  int extraout_ECX;
  uint extraout_EDX;
  uint unaff_EBX;
  
  if ((*(byte *)(param_1 + 0x24) & 2) != 0) {
    thunk_FUN_00075302();
    param_3 = extraout_ECX;
    param_2 = extraout_EDX;
  }
  if ((param_2 == 0) || (unaff_EBX <= param_2)) {
    *(undefined4 *)(param_1 + 4) = 0;
    *(undefined4 *)(param_1 + 8) = 0;
    *(byte *)(param_1 + 0x24) = *(byte *)(param_1 + 0x24) & 0xfc;
    *(byte *)(param_1 + 0x24) = *(byte *)(param_1 + 0x24) | 1;
    return;
  }
  *(uint *)(param_1 + 4) = param_2;
  puVar1 = (uint *)(param_1 + 0x24);
  *(uint *)(param_1 + 8) = unaff_EBX;
  *(byte *)puVar1 = *(byte *)puVar1 & 0xfd;
  *puVar1 = *puVar1 | (uint)(param_3 != 0) * 2;
  *(byte *)(param_1 + 0x24) = *(byte *)(param_1 + 0x24) & 0xfe;
  return;
}

