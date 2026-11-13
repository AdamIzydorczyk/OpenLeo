
undefined4 * __regparm3 FUN_000443b0(undefined4 *param_1)

{
  bool bVar1;
  int iVar2;
  undefined4 uVar3;
  undefined1 local_34;
  undefined4 local_33;
  undefined4 local_2f;
  undefined **local_2b;
  undefined4 local_27;
  undefined4 local_23;
  char *local_1c;
  undefined **local_18;
  int local_14;
  
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[3] = &PTR_LAB_00098278;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = &PTR_LAB_00098278;
  param_1[7] = 0;
  param_1[8] = 0;
  param_1[9] = &PTR_LAB_00098278;
  param_1[10] = 0;
  param_1[0xb] = 0;
  param_1[0xc] = &PTR_LAB_00098278;
  param_1[0xd] = 0;
  param_1[0xe] = 0;
  param_1[0xf] = &PTR_LAB_00098278;
  param_1[0x10] = 0;
  param_1[0x11] = 0;
  param_1[0x12] = &PTR_LAB_00098278;
  param_1[0x13] = 0;
  *param_1 = &PTR_LAB_000a0c5c;
  local_33 = 0;
  local_2f = 0;
  local_2b = &PTR_FUN_000961fc;
  local_27 = 0;
  local_23 = 0;
  FUN_00049820();
  local_1c = (char *)0x0;
  local_18 = &PTR_FUN_000961fc;
  FUN_00057d30();
  if ((local_1c == (char *)0x0) || (*local_1c == '\0')) {
    bVar1 = true;
  }
  else {
    bVar1 = false;
  }
  if (bVar1) {
LAB_000444e5:
    param_1[0x14] = 1;
  }
  else {
    iVar2 = FUN_00076d20();
    if (iVar2 != 0) goto LAB_000444e5;
    param_1[0x14] = 0;
  }
  FUN_00057d30();
  if ((local_1c == (char *)0x0) || (*local_1c == '\0')) {
    bVar1 = true;
  }
  else {
    bVar1 = false;
  }
  if (!bVar1) {
    iVar2 = FUN_00076d20();
    if (iVar2 == 0) {
      param_1[0x15] = 1;
      goto LAB_00044549;
    }
  }
  param_1[0x15] = 0;
LAB_00044549:
  FUN_0004a0b0();
  FUN_00049950(&local_34,&DAT_000917f3,&local_14);
  param_1[0x16] = 0;
  param_1[0x17] = 0;
  param_1[0x18] = 0;
  param_1[0x19] = 0;
  param_1[0x1c] = (int)(100 / (longlong)(local_14 + -1));
  uVar3 = FUN_00044370();
  param_1[0x1d] = uVar3;
  FUN_0004a0b0();
  uVar3 = FUN_00044370();
  param_1[0x1f] = uVar3;
  if (param_1[0x14] != 0) {
    FUN_0004a0b0();
    uVar3 = FUN_00044370();
    param_1[0x1e] = uVar3;
    FUN_0004a0b0();
    uVar3 = FUN_00044370();
    param_1[0x20] = uVar3;
    FUN_0004a0b0();
    uVar3 = FUN_00044370();
    param_1[0x21] = uVar3;
  }
  if (param_1[0x15] != 0) {
    FUN_0004a0b0();
    uVar3 = FUN_00044370();
    param_1[0x22] = uVar3;
  }
  FUN_00057a00();
  FUN_00049a10();
  FUN_00057a00();
  return param_1;
}

