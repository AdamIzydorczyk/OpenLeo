
undefined4 __regparm3 FUN_0007b82c(undefined4 param_1,uint param_2)

{
  code *pcVar1;
  undefined4 *puVar2;
  int unaff_EBX;
  uint local_2c;
  undefined4 local_28;
  
  puVar2 = (undefined4 *)FUN_0007b791();
  pcVar1 = *(code **)(unaff_EBX + 1);
  for (; local_2c < param_2; local_2c = local_2c + 1) {
    (*pcVar1)();
  }
  *puVar2 = *(undefined4 *)*puVar2;
  return local_28;
}

