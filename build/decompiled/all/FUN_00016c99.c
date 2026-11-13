
undefined8 __regparm3 FUN_00016c99(int param_1,undefined4 param_2)

{
  short sVar1;
  byte unaff_BL;
  
  sVar1 = (short)param_1;
  out(sVar1 + 0x103,0x43);
  out(sVar1 + 0x104,(short)param_2);
  out(sVar1 + 0x103,0x44);
  out(sVar1 + 0x105,(char)((uint)param_2 >> 0x10));
  out((short)(param_1 + 0x107),unaff_BL);
  return CONCAT44(param_1 + 0x107,(uint)unaff_BL);
}

