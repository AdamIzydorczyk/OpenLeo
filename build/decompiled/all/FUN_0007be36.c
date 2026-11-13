
undefined4 __regparm3 FUN_0007be36(undefined4 param_1,undefined2 param_2)

{
  code *pcVar1;
  undefined4 uVar2;
  
  if (DAT_000a9f94 == '\0') {
    return 0;
  }
  pcVar1 = (code *)swi(3);
  uVar2 = (*pcVar1)(param_2);
  return uVar2;
}

