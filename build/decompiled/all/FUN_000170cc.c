
undefined8 __regparm3 FUN_000170cc(undefined4 param_1,undefined4 param_2)

{
  char cVar1;
  undefined1 uVar2;
  int iVar3;
  uint uVar4;
  
  FUN_00016c57();
  FUN_00016c99();
  cVar1 = FUN_00016c57();
  if (cVar1 == -0x56) {
    iVar3 = 1;
    uVar4 = 0x400;
    FUN_00016c99();
    do {
      cVar1 = FUN_00016c57();
      if (cVar1 != '\0') break;
      uVar2 = FUN_00016c57();
      FUN_00016c99(uVar2);
      cVar1 = FUN_00016c57();
      if (cVar1 != -0x56) break;
      FUN_00016c99(uVar2);
      uVar4 = uVar4 + 0x400;
      iVar3 = iVar3 + 1;
    } while (uVar4 < 0x100000);
    FUN_00016c99();
  }
  else {
    iVar3 = 0;
  }
  return CONCAT44(param_2,iVar3);
}

