
void __regparm3 FUN_00056570(int *param_1)

{
  int *extraout_EDX;
  
  FUN_00056590();
  *param_1 = *extraout_EDX;
  *extraout_EDX = (int)param_1;
  param_1[1] = (int)extraout_EDX;
  if (*param_1 != 0) {
    *(int **)(*param_1 + 4) = param_1;
  }
  return;
}

