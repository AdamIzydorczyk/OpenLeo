
void __regparm3 FUN_00079c6f(undefined4 param_1,undefined4 param_2)

{
  code *pcVar1;
  char cVar2;
  
  if (DAT_000a9e60 != '\x01') {
    return;
  }
  if (DAT_000a9bf0 == '\0') {
    if (DAT_000a9bee == '\t') {
      (**(code **)(DAT_000a9ac4 + 0x30))();
    }
    else if (DAT_000a9bee == '\0') {
      pcVar1 = (code *)swi(0x21);
      (*pcVar1)();
    }
    else if (DAT_000a9bee == '\x01') {
      FUN_0007f682(param_2);
    }
    else {
      cVar2 = FUN_0007f682(param_2);
      if (cVar2 != '\x01') {
        FUN_00079ce9();
      }
    }
  }
  else {
    FUN_00079d24();
  }
  DAT_000a9e60 = 0;
  return;
}

