
void FUN_00016e13(void)

{
  int iVar1;
  int extraout_ECX;
  undefined8 uVar2;
  
  iVar1 = DAT_000a886d;
  while( true ) {
    while( true ) {
      if (iVar1 == 0) {
        return;
      }
      FUN_00016d17();
      uVar2 = FUN_00016d17();
      if ((int)uVar2 + iVar1 == (int)((ulonglong)uVar2 >> 0x20)) break;
      iVar1 = FUN_00016d17();
    }
    FUN_00016d17();
    FUN_00016d17();
    FUN_00016ce1();
    FUN_00016ce1();
    if (extraout_ECX == 0) break;
    FUN_00016ce1();
  }
  return;
}

