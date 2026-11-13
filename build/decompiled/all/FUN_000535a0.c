
void __regparm3 FUN_000535a0(int param_1,int param_2)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int unaff_EBX;
  int iVar2;
  
  iVar1 = *(int *)(param_1 + 0x5c);
  if (param_2 != iVar1) {
    (**(code **)(*(int *)(*(int *)(*(int *)(param_1 + 0x50) + iVar1 * 4) + 0x44) + 0x20))();
    *(undefined4 *)(extraout_ECX + 0x9d) = 0;
    *(undefined4 *)(extraout_ECX + 0x95) = 1;
    (**(code **)(*(int *)(extraout_ECX + 8) + 0xa4))();
    (**(code **)(*(int *)(*(int *)(*(int *)(extraout_ECX_00 + 0x50) + param_2 * 4) + 0x44) + 0x1c))
              ();
    *(int *)(extraout_ECX_01 + 0x5c) = param_2;
    (**(code **)(*(int *)(extraout_ECX_01 + 8) + 0xac))();
    return;
  }
  iVar1 = *(int *)(iVar1 * 4 + *(int *)(param_1 + 0x50));
  if (*(int *)(iVar1 + 4) != unaff_EBX) {
    *(int *)(iVar1 + 4) = unaff_EBX;
    (**(code **)(*(int *)(param_1 + 8) + 0xac))();
    param_1 = extraout_ECX_02;
  }
  iVar2 = *(int *)(param_1 + 0x9d) + 1;
  *(int *)(param_1 + 0x9d) = iVar2;
  iVar1 = (int)*(short *)(*(int *)(*(int *)(param_1 + 0x5c) * 4 + *(int *)(param_1 + 0x50)) + 0xc);
  if ((iVar1 != 0) && (iVar1 <= iVar2)) {
    *(byte *)(param_1 + 0x61) = *(byte *)(param_1 + 0x61) | 2;
  }
  return;
}

