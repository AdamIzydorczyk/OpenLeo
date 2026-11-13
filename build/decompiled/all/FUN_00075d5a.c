
longlong __regparm3 FUN_00075d5a(undefined4 param_1,uint param_2)

{
  PTR_FUN_000a9b68 = FUN_00075d1f;
  if (DAT_000cf5b4 < 0x20) {
    *(undefined4 *)(&DAT_000cf534 + DAT_000cf5b4 * 4) = param_1;
    DAT_000cf5b4 = DAT_000cf5b4 + 1;
    return (ulonglong)param_2 << 0x20;
  }
  return CONCAT44(param_2,0xffffffff);
}

