
undefined4 __regparm1 FUN_000823f4(undefined4 param_1,undefined4 param_2,uint param_3)

{
  ushort in_FPUStatusWord;
  
  if ((param_3 & 0x7ff00000) == 0x7ff00000) {
    return param_1;
  }
  if ((in_FPUStatusWord & 0x3800) == 0) {
    FUN_0008236f();
    return param_1;
  }
  FUN_0008236f();
  return param_1;
}

