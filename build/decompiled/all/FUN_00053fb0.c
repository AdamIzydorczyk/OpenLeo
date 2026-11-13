
void __regparm3 FUN_00053fb0(int *param_1,int param_2)

{
  uint unaff_EBX;
  
  while (((param_1[1] & unaff_EBX) == 0 || ((*(uint *)(param_2 + 4) & unaff_EBX) == 0))) {
    if ((int *)*param_1 == (int *)0x0) {
      *param_1 = param_2;
      return;
    }
    param_1 = (int *)*param_1;
  }
  FUN_0005c370();
  return;
}

