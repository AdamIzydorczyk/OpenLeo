
longlong __regparm3 FUN_0001fef0(undefined4 param_1,uint param_2)

{
  int iVar1;
  
  iVar1 = FUN_00076a69();
  if (iVar1 == 0) {
    iVar1 = FUN_0005c320();
    if (iVar1 != 0) {
      iVar1 = FUN_0001e0a0();
      *(undefined ***)(iVar1 + 8) = &PTR_LAB_00096960;
      *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_00096958;
      return CONCAT44(param_2,iVar1);
    }
  }
  else {
    iVar1 = FUN_00076a69();
    if ((iVar1 == 0) && (iVar1 = FUN_0005c320(), iVar1 != 0)) {
      iVar1 = FUN_0001e0a0();
      *(undefined ***)(iVar1 + 8) = &PTR_LAB_00097a98;
      *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_00097a90;
      *(undefined4 *)(iVar1 + 0xf6) = 10000;
      return CONCAT44(param_2,iVar1);
    }
  }
  return (ulonglong)param_2 << 0x20;
}

