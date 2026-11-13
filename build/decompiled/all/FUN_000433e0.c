
void __regparm3 FUN_000433e0(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int local_24;
  int local_1c;
  
  local_24 = 0;
  for (local_1c = 0; local_1c < *(short *)(param_1 + 0x19); local_1c = local_1c + 1) {
    uVar2 = *(undefined4 *)(*(int *)(param_1 + 0x1f) + local_24 + 4);
    for (iVar1 = 0; iVar1 < *(short *)(param_1 + 0x17); iVar1 = iVar1 + 1) {
      FUN_00043250(uVar2);
    }
    local_24 = local_24 + 8;
  }
  uVar2 = *(undefined4 *)(*(int *)(param_1 + 0x1f) + 4 + *(short *)(param_1 + 0x19) * 8);
  for (iVar1 = 0; iVar1 < *(short *)(param_1 + 0x17); iVar1 = iVar1 + 1) {
    FUN_00043250(uVar2);
  }
  return;
}

