
undefined4 FUN_00040430(void)

{
  int *piVar1;
  int *extraout_ECX;
  undefined4 extraout_ECX_00;
  
  piVar1 = (int *)FUN_00040290();
  *piVar1 = (int)&PTR_LAB_000a0ac4;
  (**(code **)(*piVar1 + 0x20))();
  (**(code **)(*extraout_ECX + 0x18))();
  return extraout_ECX_00;
}

