
void __regparm3 FUN_00055e00(int *param_1)

{
  bool bVar1;
  byte bVar2;
  int iVar3;
  int iVar4;
  int extraout_ECX;
  int extraout_ECX_00;
  undefined1 local_48;
  undefined4 local_47;
  undefined4 local_43;
  undefined **local_3f;
  undefined4 local_3b;
  undefined4 local_37;
  char *local_30;
  undefined **local_2c;
  undefined4 local_28;
  undefined **local_24;
  undefined4 local_20;
  int local_1c;
  byte local_18;
  
  bVar2 = local_18 | 1;
  if (*param_1 != 0) {
    local_18 = local_18 & 0xfe;
    local_28 = 0;
    local_24 = &PTR_FUN_00096200;
    FUN_00057990();
    local_24 = &PTR_FUN_000961fc;
    iVar3 = FUN_00057c60();
    bVar2 = local_18;
    if (iVar3 == -1) {
      bVar1 = true;
      goto LAB_00055e66;
    }
  }
  local_18 = bVar2;
  bVar1 = false;
LAB_00055e66:
  if ((local_18 & 1) == 0) {
    FUN_00057a00();
  }
  if (bVar1) {
    local_47 = 0;
    local_43 = 0;
    local_3f = &PTR_FUN_000961fc;
    local_3b = 0;
    local_37 = 0;
    FUN_000497b0();
    local_30 = (char *)0x0;
    local_2c = &PTR_FUN_000961fc;
    while( true ) {
      FUN_00057d30();
      if ((local_30 == (char *)0x0) || (*local_30 == '\0')) {
        bVar1 = true;
      }
      else {
        bVar1 = false;
      }
      if (bVar1) break;
      FUN_00049920(&local_48,&DAT_000922df,&local_20);
      FUN_00057bb0();
      iVar3 = extraout_ECX;
      local_1c = extraout_ECX;
      while (iVar3 < param_1[0x13]) {
        iVar3 = FUN_00076a69();
        if (iVar3 == 0) {
          iVar3 = FUN_0005c320();
          local_1c = 0;
          if (iVar3 != 0) {
            local_1c = FUN_00055a20();
          }
          *(undefined4 *)(local_1c + 8) = local_20;
          break;
        }
        iVar3 = extraout_ECX_00 + 1;
      }
      if (local_1c == 0) {
        iVar4 = FUN_0005c320();
        iVar3 = 0;
        if (iVar4 != 0) {
          iVar3 = FUN_00055ae0();
        }
        local_1c = iVar3;
      }
      if (param_1[0x13] == param_1[0x14]) {
        FUN_0001ccb0();
      }
      iVar3 = param_1[0x13];
      param_1[0x13] = iVar3 + 1;
      *(int *)(iVar3 * 4 + param_1[0x11]) = local_1c;
    }
    if (param_1[0x13] < param_1[0x14]) {
      FUN_0001ccb0();
    }
    FUN_00057a00();
    FUN_00049a10();
    FUN_00057a00();
  }
  return;
}

