
void __regparm3 FUN_00044660(int param_1)

{
  int extraout_ECX;
  int iVar1;
  int iVar2;
  
  iVar2 = DAT_000cef40;
  *(int *)(param_1 + 0x4c) = *(int *)(param_1 + 0x4c) + 1;
  iVar2 = *(int *)(*(int *)(iVar2 + 0x394) + 0xad);
  iVar1 = iVar2;
  if (iVar2 < 0xb) {
    iVar1 = 10;
  }
  if (iVar2 < 0) {
    iVar2 = 0;
  }
  if (((*(uint *)(param_1 + 0x4c) / (uint)(iVar1 / 5) & 1) != 0) || (0x50 < iVar2)) {
    (**(code **)(*(int *)(*(int *)(*(int *)(*(int *)(param_1 + 0x74) + 8) +
                                  ((100 - iVar2) / *(int *)(param_1 + 0x70)) * 4) + 0x33) + 0x10))
              (10);
    param_1 = extraout_ECX;
  }
  (**(code **)(*(int *)(*(int *)(*(int *)(*(int *)(param_1 + 0x7c) + 8) + DAT_000a94f0 * 4) + 0x33)
              + 0x10))();
  return;
}

