
void __regparm3 FUN_0004a190(int param_1,int param_2)

{
  int iVar1;
  int extraout_ECX;
  int iVar2;
  int extraout_ECX_00;
  int iVar3;
  int iVar4;
  
  FUN_0005b860();
  iVar4 = 0;
  iVar3 = 0;
  FUN_0005b9e0();
  FUN_0005bdd0();
  do {
    FUN_0001dba0();
    iVar1 = DAT_000cf1b2;
    FUN_0001dba0();
    iVar1 = DAT_000cf1b2 - iVar1;
    iVar2 = extraout_ECX;
    if (iVar1 < extraout_ECX) {
      iVar2 = iVar1;
    }
    if (iVar4 < iVar1) {
      iVar4 = iVar1;
    }
    iVar3 = iVar3 + 1;
  } while (iVar3 < 5);
  FUN_0005be00();
  if (iVar4 - iVar2 < 3) {
    *(int *)(param_1 + 8) = extraout_ECX_00 * param_2 + -1;
  }
  else {
    *(undefined4 *)(param_1 + 8) = 0x1b;
  }
  *(undefined4 *)(param_1 + 4) = 0;
  FUN_0005b8f0();
  return;
}

