
longlong __regparm3 FUN_00029b00(undefined4 param_1,uint param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_00076a69();
  if ((iVar1 == 0) && (iVar1 = FUN_0005c320(), iVar1 != 0)) {
    uVar2 = FUN_00027030();
    return CONCAT44(param_2,uVar2);
  }
  return (ulonglong)param_2 << 0x20;
}

