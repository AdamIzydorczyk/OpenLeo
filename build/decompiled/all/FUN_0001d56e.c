
undefined8 __regparm2 FUN_0001d56e(undefined4 param_1,undefined4 param_2,undefined1 *param_3)

{
  undefined1 uVar1;
  int iVar2;
  
  out(0x3c7,0);
  iVar2 = 0x300;
  while (iVar2 != 0) {
    iVar2 = iVar2 + -1;
    uVar1 = in(0x3c9);
    *param_3 = uVar1;
    param_3 = param_3 + 1;
  }
  return CONCAT44(param_2,param_1);
}

