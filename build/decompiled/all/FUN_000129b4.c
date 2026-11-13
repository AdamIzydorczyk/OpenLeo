
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

longlong __regparm3 FUN_000129b4(undefined4 param_1,uint param_2)

{
  undefined4 uVar1;
  short sVar2;
  int iVar3;
  undefined8 uVar4;
  
  if (DAT_000ae670 == 0) {
    iVar3 = 1;
    DAT_000ae66c = DAT_000ae668;
    while( true ) {
      sVar2 = (short)iVar3;
      if (DAT_000ae66c == (undefined4 *)0x0) break;
      uVar4 = (*(code *)DAT_000ae66c[3])();
      sVar2 = (short)((ulonglong)uVar4 >> 0x20);
      if ((int)uVar4 != 0) break;
      DAT_000ae66c = (undefined4 *)*DAT_000ae66c;
      iVar3 = (int)((ulonglong)uVar4 >> 0x20) + 1;
    }
    if (DAT_000ae66c == (undefined4 *)0x0) {
      _DAT_000c3fb0 = s_You_don_t_have_any_of_the_suppor_00090234;
      return (ulonglong)param_2 << 0x20;
    }
    DAT_000ae670 = sVar2;
  }
  sVar2 = 1;
  DAT_000ae66c = DAT_000ae668;
  while( true ) {
    if ((DAT_000ae66c == (undefined4 *)0x0) || (sVar2 == DAT_000ae670)) break;
    DAT_000ae66c = (undefined4 *)*DAT_000ae66c;
    sVar2 = sVar2 + 1;
  }
  if (DAT_000ae66c == (undefined4 *)0x0) {
    _DAT_000c3fb0 = s_Device_number_out_of_range_00090266;
    return (ulonglong)param_2 << 0x20;
  }
  uVar1 = (*(code *)DAT_000ae66c[6])();
  return CONCAT44(param_2,uVar1);
}

