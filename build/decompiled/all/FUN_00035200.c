
longlong __regparm3 FUN_00035200(undefined4 param_1,uint param_2)

{
  int iVar1;
  
  iVar1 = FUN_00076a69();
  if ((iVar1 == 0) && (iVar1 = FUN_0005c320(), iVar1 != 0)) {
    iVar1 = FUN_000500b0();
    *(undefined4 *)(iVar1 + 0x12a) = 0;
    *(undefined4 *)(iVar1 + 0x12e) = 0;
    *(undefined4 *)(iVar1 + 0x132) = 0;
    *(undefined ***)(iVar1 + 8) = &PTR_LAB_0009e134;
    *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009e12c;
    return CONCAT44(param_2,iVar1);
  }
  return (ulonglong)param_2 << 0x20;
}

