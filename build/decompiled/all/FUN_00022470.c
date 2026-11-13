
void __regparm3 FUN_00022470(int *param_1)

{
  int extraout_ECX;
  
  if (*param_1 != 0) {
    if (0 < param_1[2]) {
      do {
        (**(code **)(param_1[4] + 4))();
      } while (extraout_ECX < param_1[2]);
    }
    thunk_FUN_0005c370();
    *param_1 = 0;
    param_1[2] = 0;
    param_1[3] = 0;
  }
  return;
}

