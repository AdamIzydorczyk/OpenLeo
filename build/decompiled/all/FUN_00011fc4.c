
undefined8 __regparm3 FUN_00011fc4(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined8 uVar2;
  
  iVar1 = FUN_00011f18();
  if (iVar1 == 0) {
    return CONCAT44(param_2,0xffff);
  }
  FUN_00011f40();
  uVar2 = FUN_00011f40();
  return CONCAT44(param_2,(uint)uVar2 | (uint)((ulonglong)uVar2 >> 0x20));
}

