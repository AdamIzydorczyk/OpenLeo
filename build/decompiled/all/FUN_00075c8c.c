
void __regparm3 FUN_00075c8c(undefined4 param_1,undefined4 param_2)

{
  code *pcVar1;
  
  if ((DAT_000a9bee < 2) || (8 < DAT_000a9bee)) {
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)(param_2);
    return;
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}

