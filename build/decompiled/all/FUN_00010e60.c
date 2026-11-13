
undefined8 __regparm3 FUN_00010e60(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  iVar1 = DAT_000adea4;
  while (iVar1 != 0) {
    iVar1 = FUN_00010884();
    iVar2 = iVar2 + iVar1;
    iVar1 = FUN_00010884();
  }
  return CONCAT44(param_2,iVar2);
}

