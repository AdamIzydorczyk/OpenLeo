
longlong __regparm3 FUN_00015230(undefined4 param_1,uint param_2)

{
  byte *pbVar1;
  
  pbVar1 = DAT_000afb80;
  if (DAT_000afb80 < DAT_000afb7c) {
    DAT_000afb80 = DAT_000afb80 + 1;
    return CONCAT44(param_2,(uint)*pbVar1);
  }
  return (ulonglong)param_2 << 0x20;
}

