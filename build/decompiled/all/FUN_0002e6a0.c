
void __regparm3 FUN_0002e6a0(int param_1)

{
  int iVar1;
  
  if (*(int *)(param_1 + 0x136) == 0) {
    FUN_0002e590();
    FUN_0002e5f0();
    *(undefined4 *)(param_1 + 0x136) = 1;
  }
  FUN_00050970();
  if (*(int *)(param_1 + 4) < 0xbe) {
    if (*(int *)(param_1 + 0x12e) == 0) {
      FUN_0002e5f0();
    }
  }
  else if (*(int *)(param_1 + 0x132) == 0) {
    FUN_0002e5d0();
  }
  iVar1 = *(int *)(param_1 + 0x5c);
  if (iVar1 == 6) {
    if (*(int *)(*(int *)(*(int *)(param_1 + 0x50) + 0x18) + 4) == 7) {
      FUN_0002e470();
    }
  }
  else if (iVar1 == 0xc) {
    if ((*(int *)(*(int *)(*(int *)(param_1 + 0x50) + 0x30) + 4) == 1) &&
       (*(int *)(param_1 + 0x146) == 0)) {
      FUN_0002e500();
      return;
    }
  }
  else if (iVar1 == 0x11) {
    if (*(int *)(*(int *)(*(int *)(param_1 + 0x50) + 0x44) + 4) == 7) {
      FUN_0002e470();
    }
    if (*(int *)(*(int *)(*(int *)(param_1 + 0x50) + *(int *)(param_1 + 0x5c) * 4) + 4) == 0xe) {
LAB_0002e794:
      FUN_00052b10();
      return;
    }
  }
  else {
    if (iVar1 == 0x13) {
      if (*(int *)(*(int *)(*(int *)(param_1 + 0x50) + 0x4c) + 4) < 10) {
        FUN_00052b10();
        if (*(int *)(param_1 + 0x14a) != 0) {
          FUN_00052b10();
        }
        FUN_0002e4c0();
        return;
      }
      goto LAB_0002e794;
    }
    if (((iVar1 == 0x12) || (iVar1 == 0xb)) || (iVar1 == 0x23)) {
      FUN_00052b10();
      if (*(int *)(param_1 + 0x14a) != 0) {
        FUN_00052b10();
      }
      FUN_0002e4c0();
      if (*(int *)(param_1 + 0x1ca) != -1) {
        FUN_0002ddf0();
        (**(code **)(*(int *)(param_1 + 8) + 4))();
        return;
      }
    }
    else {
      if ((((*(byte *)(param_1 + 99) & 8) != 0) && (*(int *)(param_1 + 0x14a) == 0)) &&
         (*(int *)(param_1 + 0x146) != 0)) {
        FUN_0002e530();
        *(undefined4 *)(*(int *)(param_1 + 0x13e) + 0xad) = *(undefined4 *)(param_1 + 0x142);
      }
      *(undefined4 *)(param_1 + 0x14e) = 0;
    }
  }
  return;
}

