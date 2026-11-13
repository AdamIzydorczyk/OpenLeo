
undefined8 __regparm3 FUN_00018870(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined8 uVar1;
  undefined6 uVar2;
  
  uVar2 = FUN_00075c5a(param_2,param_3);
  DAT_000c4034 = (undefined2)((uint6)uVar2 >> 0x20);
  DAT_000c4030 = (undefined4)uVar2;
  uVar1 = FUN_00075c8c();
  DAT_000c3fe4 = (int)((ulonglong)uVar1 >> 0x20);
  out(0x43,0x36);
  out(0x40,0x95);
  out(0x40,0x42);
  return CONCAT44(param_2,CONCAT31((int3)((ulonglong)uVar1 >> 8),0x42));
}

