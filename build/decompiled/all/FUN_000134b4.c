
void __regparm3 FUN_000134b4(byte *param_1,undefined4 param_2,undefined4 param_3)

{
  byte *extraout_EDX;
  byte *extraout_EDX_00;
  ushort uVar1;
  
  if (*(int *)(param_1 + 0xd5) != 0) {
    for (uVar1 = 0; (uint)uVar1 < (uint)*param_1; uVar1 = uVar1 + 1) {
      if (-1 < *(int *)((uint)uVar1 * 0x1d + 0x19 + *(int *)(param_1 + 0xd5)) >> 0x10) {
        FUN_000129a4();
        param_1 = extraout_EDX;
      }
    }
    FUN_0001319c(param_2,param_3);
    param_1 = extraout_EDX_00;
  }
  if (*(int *)(param_1 + 0xd1) != 0) {
    FUN_00075302();
  }
  return;
}

