
void __regparm3 FUN_00023850(undefined4 *param_1,undefined4 param_2)

{
  undefined4 unaff_EBX;
  
  *param_1 = param_2;
  param_1[1] = unaff_EBX;
  (**(code **)(param_1[2] + 0x54))();
  return;
}

