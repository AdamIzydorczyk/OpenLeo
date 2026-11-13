
undefined1 __regparm3 FUN_00040710(undefined4 *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined1 extraout_CL;
  
  iVar2 = FUN_000406e0(param_3);
  uVar1 = 0;
  if (iVar2 != 0) {
    out((short)*param_1 + 0xc,extraout_CL);
    uVar1 = extraout_CL;
  }
  return uVar1;
}

