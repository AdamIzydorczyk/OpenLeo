
undefined4 __regparm3 thunk_FUN_0007c1b4(undefined4 param_1,undefined4 param_2)

{
  code *pcVar1;
  undefined4 uVar2;
  
  if (DAT_000cf508 == (code *)0x0) {
    if (DAT_000cf500 == 0) {
      FUN_00078b55(1);
      pcVar1 = (code *)swi(0x21);
      uVar2 = (*pcVar1)();
      return uVar2;
    }
  }
  else {
    (*DAT_000cf508)(DAT_000cf508,param_2);
  }
  uVar2 = FUN_00077bdc();
  return uVar2;
}

