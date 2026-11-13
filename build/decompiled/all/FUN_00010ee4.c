
void __regparm3 FUN_00010ee4(int param_1)

{
  uint uVar1;
  int iVar2;
  uint extraout_ECX;
  uint uVar3;
  uint extraout_ECX_00;
  uint uVar4;
  uint uVar5;
  
  uVar1 = DAT_000adea4;
  if (param_1 != 0) {
    FUN_000108c0();
    uVar3 = extraout_ECX;
    uVar5 = 0;
    while ((uVar4 = uVar1, uVar4 != 0 && (uVar4 <= uVar3))) {
      uVar1 = FUN_00010884();
      uVar3 = extraout_ECX_00;
      uVar5 = uVar4;
    }
    if (uVar5 == 0) {
      DAT_000adea4 = uVar3;
    }
    else {
      FUN_000108c0();
    }
    FUN_000108c0();
    iVar2 = FUN_00010e88();
    if (iVar2 != 0) {
      FUN_00010eac();
    }
    iVar2 = FUN_00010e88();
    if (iVar2 != 0) {
      FUN_00010eac();
    }
  }
  return;
}

