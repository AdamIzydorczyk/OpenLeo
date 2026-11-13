
undefined8 __regparm3 FUN_000781d4(int param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  
  iVar2 = 0;
  while( true ) {
    cVar1 = FUN_00077e89();
    if (((&DAT_000a68b4)[(byte)(cVar1 + 1)] & 2) == 0) break;
    iVar2 = iVar2 + 1;
  }
  if ((*(byte *)(param_1 + 0x10) & 2) == 0) {
    FUN_00077e90();
  }
  return CONCAT44(param_2,iVar2);
}

