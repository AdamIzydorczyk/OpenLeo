
undefined8 __regparm3 FUN_00012d14(undefined4 param_1,undefined4 param_2)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 uVar3;
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int extraout_EDX_02;
  uint uVar4;
  undefined8 uVar5;
  int local_1c;
  
  iVar2 = FUN_000131b8();
  uVar3 = 0;
  if (iVar2 != 0) {
    iVar2 = FUN_00013268();
    uVar3 = 0;
    if (iVar2 != 0) {
      DAT_000aee8c = FUN_0001317c();
      uVar3 = 0;
      if (DAT_000aee8c != 0) {
        for (local_1c = 0; local_1c < (int)(uint)DAT_000aee99; local_1c = local_1c + 1) {
          for (uVar4 = 0; uVar4 < (uint)DAT_000aee94 << 6; uVar4 = uVar4 + 1) {
            uVar1 = FUN_00016a18();
            *(undefined1 *)(extraout_EDX + DAT_000aee8c) = uVar1;
            uVar1 = FUN_00016a18();
            *(undefined1 *)(extraout_EDX_00 + 1 + DAT_000aee8c) = uVar1;
            uVar1 = FUN_00016a18();
            *(undefined1 *)(extraout_EDX_01 + 2 + DAT_000aee8c) = uVar1;
            uVar1 = FUN_00016a18();
            *(undefined1 *)(extraout_EDX_02 + -1 + DAT_000aee8c) = uVar1;
          }
          for (iVar2 = 0; iVar2 < (int)(uint)DAT_000aee94; iVar2 = iVar2 + 1) {
            uVar5 = FUN_00012ce0();
            *(int *)((int)((ulonglong)uVar5 >> 0x20) + DAT_000aefda) = (int)uVar5;
            if ((int)uVar5 == 0) {
              uVar3 = 0;
              goto LAB_00012e25;
            }
          }
        }
        uVar3 = 1;
      }
    }
  }
LAB_00012e25:
  return CONCAT44(param_2,uVar3);
}

