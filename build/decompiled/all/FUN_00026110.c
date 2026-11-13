
undefined8 __regparm3 FUN_00026110(undefined4 param_1,undefined4 param_2)

{
  int *piVar1;
  int *extraout_ECX;
  uint uVar2;
  
  FUN_00052d30();
  piVar1 = (int *)FUN_00052d30();
  uVar2 = *piVar1 - *extraout_ECX >> 0x1f;
  return CONCAT44(param_2,(uint)((int)((*piVar1 - *extraout_ECX ^ uVar2) - uVar2) < 100));
}

