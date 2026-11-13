
undefined4 __regparm3 FUN_00016fc1(int param_1)

{
  uint uVar1;
  int iVar2;
  int *extraout_ECX;
  int *extraout_ECX_00;
  int *extraout_ECX_01;
  int *extraout_ECX_02;
  int *piVar3;
  int iVar4;
  undefined8 uVar5;
  uint local_18;
  
  local_18 = param_1 + 0x1fU & 0xffffffe0;
  iVar4 = DAT_000a886d;
  while( true ) {
    if (iVar4 == 0) {
      return 3;
    }
    uVar1 = FUN_00016d17();
    if (local_18 <= uVar1) break;
    iVar4 = FUN_00016d17();
  }
  if (uVar1 - local_18 < 0xc) {
    iVar2 = FUN_00016d17();
    uVar5 = FUN_00016d17();
    piVar3 = extraout_ECX;
    if ((int)((ulonglong)uVar5 >> 0x20) != 0) {
      FUN_00016ce1();
      piVar3 = extraout_ECX_00;
    }
    if ((int)uVar5 == 0) {
      DAT_000a886d = iVar2;
    }
    else {
      FUN_00016ce1();
      piVar3 = extraout_ECX_01;
    }
  }
  else {
    iVar4 = iVar4 + (uVar1 - local_18);
    FUN_00016ce1();
    piVar3 = extraout_ECX_02;
  }
  *piVar3 = iVar4;
  return 1;
}

