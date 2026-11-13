
undefined8 __regparm3 FUN_00040770(undefined4 *param_1,undefined4 param_2)

{
  int iVar1;
  
  out((short)*param_1 + 6,1);
  FUN_00040750();
  out((short)*param_1 + 6,0);
  iVar1 = FUN_00040730();
  return CONCAT44(param_2,(uint)(iVar1 == 0xaa));
}

