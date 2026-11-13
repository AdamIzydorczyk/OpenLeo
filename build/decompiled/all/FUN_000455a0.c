
int __regparm3 FUN_000455a0(int *param_1,int *param_2)

{
  int *unaff_EBX;
  
  return (((*unaff_EBX - *param_1) * (param_2[1] - param_1[1])) / (*param_2 - *param_1) + param_1[1]
         ) - unaff_EBX[1];
}

