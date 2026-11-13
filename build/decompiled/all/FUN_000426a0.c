
void __regparm2 FUN_000426a0(int param_1,int param_2,undefined4 param_3,int param_4)

{
  int local_38;
  int local_34;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  
  if (*(int *)(param_1 + 0x1f) != 0) {
    if (param_4 == 0) {
      local_38 = param_4;
      local_34 = param_2;
      for (local_1c = param_4; local_1c < *(short *)(param_1 + 0x19); local_1c = local_1c + 1) {
        if (*(char *)(param_1 + 0x14) == '\0') {
          FUN_0001d9b8(*(undefined4 *)(*(int *)(param_1 + 0x1f) + local_38 + 4),local_34);
        }
        else {
          FUN_0001dc80(*(undefined4 *)(*(int *)(param_1 + 0x1f) + local_38 + 4),local_34);
        }
        local_34 = local_34 + 0x50;
        local_38 = local_38 + 8;
      }
      if (*(char *)(param_1 + 0x14) == '\0') {
        FUN_0001d9b8(*(undefined4 *)(*(int *)(param_1 + 0x1f) + local_38 + 4),local_34);
        return;
      }
      FUN_0001dc80(*(undefined4 *)(*(int *)(param_1 + 0x1f) + local_38 + 4),local_34);
    }
    else {
      local_10 = 0;
      local_14 = (*(short *)(param_1 + 0x19) + -1) * 0x50 + (int)*(short *)(param_1 + 0x15) +
                 param_2;
      for (local_18 = 0; local_18 < *(short *)(param_1 + 0x19); local_18 = local_18 + 1) {
        if (*(char *)(param_1 + 0x14) == '\0') {
          FUN_0001daa4(*(undefined4 *)(*(int *)(param_1 + 0x1f) + local_10 + 4),local_14);
        }
        else {
          FUN_0001dda8(*(undefined4 *)(*(int *)(param_1 + 0x1f) + local_10 + 4),local_14);
        }
        local_14 = local_14 + -0x50;
        local_10 = local_10 + 8;
      }
      if (*(char *)(param_1 + 0x14) != '\0') {
        FUN_0001dda8(*(undefined4 *)(*(int *)(param_1 + 0x1f) + local_10 + 4),param_2);
        return;
      }
      FUN_0001daa4(*(undefined4 *)(*(int *)(param_1 + 0x1f) + local_10 + 4),param_2);
    }
  }
  return;
}

