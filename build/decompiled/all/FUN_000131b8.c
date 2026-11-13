
undefined8 __regparm3 FUN_000131b8(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  short sVar3;
  int iVar4;
  int local_18;
  
  sVar3 = 0;
  DAT_000aefd2 = FUN_0001317c();
  uVar2 = 0;
  if (DAT_000aefd2 != 0) {
    DAT_000aefd6 = FUN_0001317c();
    uVar2 = 0;
    if (DAT_000aefd6 != 0) {
      local_18 = 0;
      for (iVar4 = 0; iVar4 < (int)(uint)DAT_000aee99; iVar4 = iVar4 + 1) {
        *(undefined2 *)(DAT_000aefd6 + local_18) = 0x40;
        for (iVar1 = 0; iVar1 < (int)(uint)DAT_000aee94; iVar1 = iVar1 + 1) {
          *(short *)(DAT_000aefd2 + ((uint)DAT_000aee94 * iVar4 + iVar1) * 2) = sVar3;
          sVar3 = sVar3 + 1;
        }
        local_18 = local_18 + 2;
      }
      uVar2 = 1;
    }
  }
  return CONCAT44(param_2,uVar2);
}

