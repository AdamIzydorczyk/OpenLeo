
undefined4 __regparm3 FUN_00058010(int *param_1,undefined4 *param_2)

{
  uint uVar1;
  int local_18;
  int local_14;
  
  if (param_1[4] != 0) {
    if (param_1[5] != 0) {
      uVar1 = FUN_0001df34();
      if ((uVar1 & 0x10) == 0) {
        *(byte *)(param_2 + 2) = *(byte *)(param_2 + 2) | 1;
      }
      if ((uVar1 & 0x20) == 0) {
        *(byte *)(param_2 + 2) = *(byte *)(param_2 + 2) | 0x20;
      }
      FUN_0001ded4(&local_18,&local_14,1,2);
      if (local_18 < *param_1) {
        *param_2 = 0xffffffec;
      }
      else if (param_1[1] < local_18) {
        *param_2 = 0x14;
      }
      if (local_14 < param_1[2]) {
        param_2[1] = 0xffffffec;
      }
      else if (param_1[3] < local_14) {
        param_2[1] = 0x14;
      }
      return 1;
    }
    if (param_1[6] != 0) {
      uVar1 = FUN_0001df34();
      if ((uVar1 & 0x40) == 0) {
        *(byte *)(param_2 + 2) = *(byte *)(param_2 + 2) | 1;
      }
      if ((uVar1 & 0x80) == 0) {
        *(byte *)(param_2 + 2) = *(byte *)(param_2 + 2) | 0x20;
      }
      FUN_0001ded4(&local_18,&local_14,4,8);
      if (local_18 < *param_1) {
        *param_2 = 0xffffffec;
      }
      else if (param_1[1] < local_18) {
        *param_2 = 0x14;
      }
      if (local_14 < param_1[2]) {
        param_2[1] = 0xffffffec;
      }
      else if (param_1[3] < local_14) {
        param_2[1] = 0x14;
      }
      return 1;
    }
  }
  return 0;
}

