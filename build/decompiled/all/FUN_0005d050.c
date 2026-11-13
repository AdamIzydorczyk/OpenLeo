
int FUN_0005d050(void)

{
  bool bVar1;
  undefined4 *puVar2;
  int iVar3;
  int unaff_EDI;
  
  FUN_0005cfa0();
  puVar2 = (undefined4 *)FUN_0005ce60();
  if (((char *)*puVar2 == (char *)0x0) || (*(char *)*puVar2 == '\0')) {
    bVar1 = true;
  }
  else {
    bVar1 = false;
  }
  FUN_00057a00();
  if (!bVar1) {
    FUN_0005ce60();
    unaff_EDI = FUN_0005cfc0();
    FUN_00057a00();
    if (unaff_EDI == 0xc) {
      DAT_000cf460 = 0x10;
      FUN_0005ce60();
      unaff_EDI = FUN_0005cfc0();
      FUN_00057a00();
    }
  }
  FUN_0001c490();
  while( true ) {
    iVar3 = FUN_000766a7();
    if (iVar3 == 0) break;
    FUN_000766be();
  }
  return unaff_EDI;
}

