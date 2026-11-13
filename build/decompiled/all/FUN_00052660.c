
void __regparm3 FUN_00052660(undefined4 param_1,undefined4 param_2)

{
  int *extraout_EDX;
  
  FUN_00054f90(param_2);
  *(byte *)(*extraout_EDX + 0xb6) = *(byte *)(*extraout_EDX + 0xb6) & 0xdf;
  FUN_000523e0();
  return;
}

