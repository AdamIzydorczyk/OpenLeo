
void __regparm3 FUN_00057590(int *param_1)

{
  if (param_1[4] < *param_1) {
    param_1[0xb] = 0;
    param_1[4] = *param_1;
  }
  if (param_1[1] - 0x140U < (uint)param_1[4]) {
    param_1[0xb] = 0;
    param_1[4] = param_1[1] - 0x140U;
  }
  if (param_1[5] < param_1[2]) {
    param_1[0xc] = 0;
    param_1[5] = param_1[2];
  }
  if (param_1[3] + -200 < param_1[5]) {
    param_1[0xc] = 0;
    param_1[5] = param_1[3] + -200;
  }
  return;
}

