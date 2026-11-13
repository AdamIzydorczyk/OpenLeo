
undefined4 __regparm3 FUN_000582f0(char *param_1,int *param_2,undefined4 param_3)

{
  int iVar1;
  
  *param_2 = 0;
  param_2[1] = 0;
  param_2[2] = 0;
  if (*param_1 != '\0') {
    iVar1 = FUN_00051320(param_3);
    if (iVar1 == 0) {
      iVar1 = FUN_0001c550();
      if (iVar1 != 0) {
        *(byte *)((int)param_2 + 9) = *(byte *)((int)param_2 + 9) | 0x20;
      }
      iVar1 = FUN_0001c550();
      if (iVar1 != 0) {
        *(byte *)(param_2 + 2) = *(byte *)(param_2 + 2) | 0x40;
      }
      iVar1 = FUN_0001c550();
      if (iVar1 != 0) {
        *(byte *)(param_2 + 2) = *(byte *)(param_2 + 2) | 0x80;
      }
      iVar1 = FUN_0001c550();
      if (iVar1 != 0) {
        *(byte *)((int)param_2 + 9) = *(byte *)((int)param_2 + 9) | 1;
      }
      iVar1 = FUN_0001c550();
      if (iVar1 != 0) {
        *(byte *)((int)param_2 + 9) = *(byte *)((int)param_2 + 9) | 2;
      }
      iVar1 = FUN_0001c550();
      if (iVar1 != 0) {
        *(byte *)((int)param_2 + 9) = *(byte *)((int)param_2 + 9) | 0x40;
      }
      iVar1 = FUN_0001c550();
      if (iVar1 != 0) {
        *(byte *)((int)param_2 + 9) = *(byte *)((int)param_2 + 9) | 0x80;
      }
      iVar1 = FUN_0001c550();
      if (iVar1 != 0) {
        *(byte *)((int)param_2 + 10) = *(byte *)((int)param_2 + 10) | 1;
      }
      iVar1 = FUN_0001c550();
      if (iVar1 != 0) {
        *(byte *)((int)param_2 + 10) = *(byte *)((int)param_2 + 10) | 2;
      }
      iVar1 = FUN_0001c550();
      if (iVar1 != 0) {
        *(byte *)((int)param_2 + 10) = *(byte *)((int)param_2 + 10) | 4;
      }
      iVar1 = FUN_0001c550();
      if (iVar1 != 0) {
        *(byte *)((int)param_2 + 10) = *(byte *)((int)param_2 + 10) | 8;
      }
      iVar1 = FUN_0001c550();
      if (iVar1 != 0) {
        *(byte *)((int)param_2 + 9) = *(byte *)((int)param_2 + 9) | 4;
      }
      iVar1 = FUN_0001c550();
      if (iVar1 != 0) {
        *(byte *)(param_2 + 2) = *(byte *)(param_2 + 2) | 4;
      }
      iVar1 = FUN_0001c550();
      if (iVar1 != 0) {
        *(byte *)(param_2 + 2) = *(byte *)(param_2 + 2) | 8;
      }
      iVar1 = FUN_0001c530();
      if (iVar1 != 0) {
        *param_2 = -0x14;
      }
      iVar1 = FUN_0001c530();
      if (iVar1 != 0) {
        *param_2 = 0x14;
      }
      iVar1 = FUN_0001c530();
      if (iVar1 != 0) {
        param_2[1] = 0x14;
      }
      iVar1 = FUN_0001c530();
      if (iVar1 != 0) {
        param_2[1] = -0x14;
      }
      iVar1 = FUN_0001c530();
      if (iVar1 != 0) {
        *(byte *)(param_2 + 2) = *(byte *)(param_2 + 2) | 0x20;
      }
      if ((*param_2 != 0) || (param_2[1] != 0)) {
        *(byte *)(param_2 + 2) = *(byte *)(param_2 + 2) | 2;
      }
      iVar1 = FUN_0001c530();
      if (iVar1 != 0) {
        *(byte *)(param_2 + 2) = *(byte *)(param_2 + 2) | 0x10;
      }
      iVar1 = FUN_0001c530();
      if (iVar1 != 0) {
        *(byte *)(param_2 + 2) = *(byte *)(param_2 + 2) | 1;
      }
      iVar1 = FUN_0001c550();
      if (iVar1 != 0) {
        *(byte *)((int)param_2 + 9) = *(byte *)((int)param_2 + 9) | 0x10;
      }
      iVar1 = FUN_00058550();
      if (iVar1 != 0) {
        *(ushort *)(param_2 + 2) = *(ushort *)(param_2 + 2) | 0x808;
      }
      return 1;
    }
  }
  return 0;
}

