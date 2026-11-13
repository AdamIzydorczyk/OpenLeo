
void __regparm3
FUN_00014aa4(undefined1 *param_1,undefined1 param_2,undefined1 param_3,undefined1 param_4,
            undefined1 param_5,undefined4 param_6)

{
  undefined1 unaff_BL;
  
  *(undefined2 *)(param_1 + 5) = 0;
  *(undefined2 *)(param_1 + 7) = 0;
  *(undefined2 *)(param_1 + 9) = 1;
  *param_1 = param_2;
  param_1[1] = unaff_BL;
  param_1[2] = param_3;
  param_1[3] = param_4;
  param_1[4] = param_5;
  *(undefined4 *)(param_1 + 0xb) = param_6;
  return;
}

