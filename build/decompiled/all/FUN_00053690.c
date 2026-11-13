
void __regparm3 FUN_00053690(undefined4 param_1,int *param_2)

{
  undefined4 uVar1;
  int extraout_ECX;
  
  FUN_000534e0();
  if ((((param_2[1] < 1) && (-1 < param_2[3])) && (*param_2 < 1)) && (-1 < param_2[2])) {
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  *(undefined4 *)(extraout_ECX + 0x2c) = uVar1;
  return;
}

