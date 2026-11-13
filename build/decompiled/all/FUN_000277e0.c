
longdouble __regparm1 FUN_000277e0(int param_1,double param_2)

{
  return (longdouble)*(int *)(param_1 + 0x13a) +
         (((longdouble)param_2 - (longdouble)*(int *)(param_1 + 0x13e)) *
         (longdouble)(*(int *)(param_1 + 0x142) - *(int *)(param_1 + 0x13a))) /
         (longdouble)(*(int *)(param_1 + 0x146) - *(int *)(param_1 + 0x13e));
}

