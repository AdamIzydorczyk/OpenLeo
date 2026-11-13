
undefined8 __regparm3 FUN_00019740(undefined4 param_1,undefined4 param_2)

{
  undefined8 uVar1;
  
  func_0x02285690();
  FUN_000196b0();
  uVar1 = FUN_000196b0();
  return CONCAT44(param_2,CONCAT22((short)((ulonglong)uVar1 >> 0x10),(ushort)(byte)uVar1) |
                          CONCAT22((short)((ulonglong)uVar1 >> 0x30),
                                   (ushort)(byte)((ulonglong)uVar1 >> 0x20)) << 8);
}

