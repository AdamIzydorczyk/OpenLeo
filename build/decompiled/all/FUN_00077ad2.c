
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

longlong __regparm3 FUN_00077ad2(undefined4 param_1,undefined4 param_2)

{
  code *pcVar1;
  uint unaff_retaddr;
  
  _DAT_000a9b9e = (undefined2)param_2;
  _FUN_000a9b9a = param_1;
  if ((DAT_000a9bee < 2) || (7 < DAT_000a9bee)) {
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)(param_2);
  }
  else {
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
  }
  return (ulonglong)unaff_retaddr << 0x20;
}

