
longlong __regparm3 FUN_0003a430(undefined4 param_1,uint param_2)

{
  int iVar1;
  
  iVar1 = FUN_00076a69();
  if (iVar1 == 0) {
    iVar1 = FUN_0005c320();
    if (iVar1 != 0) {
      iVar1 = FUN_000500b0();
      *(undefined ***)(iVar1 + 8) = &PTR_LAB_0009f668;
      *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009f660;
      return CONCAT44(param_2,iVar1);
    }
  }
  else {
    iVar1 = FUN_00076a69();
    if ((iVar1 == 0) && (iVar1 = FUN_0005c320(), iVar1 != 0)) {
      iVar1 = FUN_000500b0();
      *(undefined ***)(iVar1 + 8) = &PTR_LAB_0009f540;
      *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009f538;
      return CONCAT44(param_2,iVar1);
    }
  }
  return (ulonglong)param_2 << 0x20;
}

