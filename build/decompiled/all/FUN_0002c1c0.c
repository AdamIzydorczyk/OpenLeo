
void __regparm3 FUN_0002c1c0(int param_1)

{
  int iVar1;
  
  if ((*(int *)(param_1 + 0xa9) != 0) && (*(int *)(param_1 + 0x5c) == 1)) {
    if ((*(byte *)(param_1 + 100) & 0x20) == 0) {
      if (((*(byte *)(param_1 + 100) & 0x40) == 0) || (iVar1 = FUN_0004b760(), 4 < iVar1))
      goto LAB_0002c257;
    }
    else {
      iVar1 = FUN_0004b6d0();
      if (iVar1 < -4) goto LAB_0002c257;
    }
    FUN_00052b10();
    FUN_00052b10();
  }
LAB_0002c257:
  if ((*(byte *)(param_1 + 0x61) & 0x40) == 0) {
    if ((*(byte *)(param_1 + 0x66) & 0x40) == 0) {
      if ((*(byte *)(param_1 + 99) & 8) == 0) {
        if ((*(byte *)(param_1 + 100) & 2) != 0) {
          *(undefined4 *)(param_1 + 0x12a) = 0;
          *(undefined4 *)(param_1 + 0x12e) = 1;
        }
      }
      else {
        *(undefined4 *)(param_1 + 0x12a) = 1;
      }
    }
    else {
      *(undefined4 *)(param_1 + 0x12e) = 0;
      *(undefined4 *)(param_1 + 0x12a) = 2;
    }
  }
  else {
    *(undefined4 *)(param_1 + 0xad) = 0xffffffff;
  }
  if ((*(int *)(DAT_000cef40 + 0x3d6) == 0) && (*(int *)(param_1 + 0x5c) != 2)) {
    (*(code *)PTR_FUN_0009827c)();
    FUN_0004b3c0();
  }
  FUN_00050970();
  return;
}

