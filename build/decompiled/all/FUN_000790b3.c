
longlong __regparm3 FUN_000790b3(undefined4 param_1,uint param_2)

{
  int *piVar1;
  undefined3 uVar2;
  ushort in_DS;
  
  if (DAT_000a9b58 != 0) {
    piVar1 = *(int **)(DAT_000a9b58 + 0x24);
    if (((1 < DAT_000a9bee) && (DAT_000a9bee < 9)) && (DAT_000a9bf0 == '\0')) {
      uVar2 = SegmentLimit((uint)in_DS);
      DAT_000a9bc0 = (ushort)uVar2 + 1;
    }
    if ((int)piVar1 + *piVar1 + 4 == DAT_000a9bc0) {
      return CONCAT44(param_2,*piVar1);
    }
  }
  return (ulonglong)param_2 << 0x20;
}

