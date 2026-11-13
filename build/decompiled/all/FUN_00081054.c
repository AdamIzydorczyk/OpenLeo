
longdouble __regparm3 FUN_00081054(int *param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = (*(code *)PTR_FUN_000ad5d8)(param_2);
  if (iVar1 == 0) {
    FUN_0008100c();
    if (*param_1 == 1) {
      FUN_00078c0d();
    }
    else {
      FUN_00078c1e();
    }
  }
  return (longdouble)*(double *)(param_1 + 6);
}

