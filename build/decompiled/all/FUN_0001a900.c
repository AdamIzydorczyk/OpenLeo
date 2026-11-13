
undefined8 __regparm3 FUN_0001a900(undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  longdouble lVar2;
  longdouble lVar3;
  
  lVar2 = (longdouble)FUN_00075d96(param_1,0);
  lVar3 = (longdouble)FUN_00075d96();
  uVar1 = (uint)ROUND(lVar3);
  DAT_000c410a = (short)(int)ROUND(lVar2);
  DAT_000c4110 = (short)uVar1;
  out(0x43,0x36);
  out(0x40,(char)uVar1);
  out(0x40,(char)(uVar1 >> 8));
  return CONCAT44(param_2,(int)(uVar1 & 0xffff) >> 8);
}

