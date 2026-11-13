
undefined4 __regparm3 FUN_0007fc6e(undefined4 param_1)

{
  bool bVar1;
  int iVar2;
  bool bVar3;
  
  bVar3 = false;
  while( true ) {
    iVar2 = FUN_0007fca6();
    if (iVar2 == 0) {
      return param_1;
    }
    if (iVar2 == 1) break;
    if (((DAT_000a9bee == '\x01') && (DAT_000a9bef == '\0')) || (DAT_000a9bee == '\t')) {
      bVar1 = false;
    }
    else {
      bVar1 = true;
    }
    if (!bVar1) {
      return 0;
    }
    if (iVar2 == 2) {
      if ((bVar3) || (iVar2 = FUN_00079284(), iVar2 == 0)) {
        return 0;
      }
      bVar3 = true;
    }
  }
  return 0;
}

