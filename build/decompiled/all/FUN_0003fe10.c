
void __regparm3 FUN_0003fe10(int *param_1)

{
  int iVar1;
  undefined4 *extraout_ECX;
  undefined4 *puVar2;
  undefined4 local_c;
  
  FUN_00053440();
  iVar1 = FUN_0005c320();
  local_c = 0;
  if (iVar1 != 0) {
    local_c = FUN_0003fa20();
  }
  puVar2 = &local_c;
  if (param_1[2] == param_1[3]) {
    FUN_00021ac0();
    puVar2 = extraout_ECX;
  }
  iVar1 = param_1[2];
  param_1[2] = iVar1 + 1;
  *(undefined4 *)(*param_1 + iVar1 * 4) = *puVar2;
  return;
}

