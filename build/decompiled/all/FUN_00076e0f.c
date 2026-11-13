
undefined8 __regparm3 FUN_00076e0f(int param_1,int param_2)

{
  int iVar1;
  
  (*(code *)PTR_FUN_000a9e14)(param_2,param_1,param_1,param_2);
  iVar1 = *(int *)(param_1 + 0x1c);
  *(undefined4 *)(iVar1 + -4) = *(undefined4 *)(param_1 + 0x18);
  if (param_2 == 0) {
    param_2 = 1;
  }
  *(int *)(iVar1 + -8) = param_2;
  verr();
  verr();
  verr();
  return CONCAT44(*(undefined4 *)(param_1 + 8),*(undefined4 *)(iVar1 + -8));
}

