
undefined8 __regparm3 FUN_0007a648(int *param_1,undefined4 param_2)

{
  int *extraout_EDX;
  int *extraout_EDX_00;
  
  if (*(int *)((int)param_1 + *(int *)(*param_1 + 4) + 8) != 0) {
    FUN_0007a9fc();
    param_1 = extraout_EDX;
  }
  if ((*(uint *)((int)param_1 + *(int *)(*param_1 + 4) + 0xc) & 0x4000) != 0) {
    FUN_0007fabf();
    FUN_0007a4e4();
    FUN_0007fabf();
    FUN_0007a4e4();
    param_1 = extraout_EDX_00;
  }
  return CONCAT44(param_2,(uint)(*(int *)((int)param_1 + *(int *)(*param_1 + 4) + 0x10) == 0));
}

