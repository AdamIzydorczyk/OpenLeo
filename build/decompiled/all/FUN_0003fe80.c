
void __regparm3 FUN_0003fe80(int *param_1)

{
  int iVar1;
  undefined4 uVar2;
  int *extraout_ECX;
  
  FUN_0003ffe0();
  iVar1 = FUN_0005c320();
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = FUN_0003fac0();
  }
  if (param_1[2] == param_1[3]) {
    FUN_00021ac0();
    param_1 = extraout_ECX;
  }
  iVar1 = param_1[2];
  param_1[2] = iVar1 + 1;
  *(undefined4 *)(*param_1 + iVar1 * 4) = uVar2;
  return;
}

