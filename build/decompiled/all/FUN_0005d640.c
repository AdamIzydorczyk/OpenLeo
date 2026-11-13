
undefined8 __regparm3 FUN_0005d640(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  uint local_18;
  
  FUN_0005d170();
  if (local_18 < 3) {
    if (local_18 != 2) {
      return CONCAT44(param_2,1);
    }
    uVar1 = FUN_0005d2f0();
  }
  else {
    if (local_18 < 4) {
      FUN_0005da60();
      FUN_0005d050();
    }
    else if (local_18 == 4) {
      uVar1 = 0;
      goto LAB_0005d6a8;
    }
    uVar1 = 1;
  }
LAB_0005d6a8:
  return CONCAT44(param_2,uVar1);
}

