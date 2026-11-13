
undefined8 __regparm2 FUN_0001d550(undefined4 param_1,undefined4 param_2,undefined1 *param_3)

{
  int iVar1;
  
  out(0x3c8,0);
  iVar1 = 0x300;
  while (iVar1 != 0) {
    iVar1 = iVar1 + -1;
    out(*param_3,0x3c9);
    param_3 = param_3 + 1;
  }
  return CONCAT44(param_2,param_1);
}

