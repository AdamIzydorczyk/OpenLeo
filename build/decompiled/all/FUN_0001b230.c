
undefined8 __regparm3 FUN_0001b230(undefined4 param_1,undefined4 param_2)

{
  undefined1 uVar1;
  undefined1 uVar2;
  uint uVar3;
  
  out(DAT_000c4072,0xff);
  uVar1 = in(DAT_000c4074);
  uVar2 = in(DAT_000c4074);
  uVar3 = (uint)CONCAT11(uVar2,uVar1);
  if (3 < DAT_000c4070) {
    uVar3 = uVar3 * 2;
  }
  return CONCAT44(param_2,uVar3);
}

