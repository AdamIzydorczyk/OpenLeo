
undefined1 __regparm3 FUN_00016c57(short param_1,undefined4 param_2)

{
  undefined1 uVar1;
  
  out(param_1 + 0x103,0x43);
  out(param_1 + 0x104,(short)param_2);
  out(param_1 + 0x103,0x44);
  out(param_1 + 0x105,(char)((uint)param_2 >> 0x10));
  uVar1 = in(param_1 + 0x107);
  return uVar1;
}

