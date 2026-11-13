
undefined8 __regparm3 FUN_0005b820(undefined4 param_1,int param_2)

{
  out(0x43,0x36);
  out(0x40,(char)param_2);
  out(0x40,(char)((uint)param_2 >> 8));
  return CONCAT44(0x40,param_2 >> 8);
}

