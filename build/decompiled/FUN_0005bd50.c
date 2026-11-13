
void __regparm3 FUN_0005bd50(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = FUN_0005bbd0(param_2);
  if (iVar1 != 0) {
    iVar1 = FUN_0005bc60();
    if (iVar1 == 0) {
      iVar1 = FUN_0005bd10();
      if (iVar1 == 0) {
        iVar1 = FUN_0005bca0();
        if (iVar1 == 0) {
          iVar1 = 1;
          goto LAB_0005bd8a;
        }
      }
    }
  }
  iVar1 = 0;
LAB_0005bd8a:
  DAT_000cf1c0 = DAT_000cf1c0 + iVar1;
  return;
}

 (GhidraScript)  