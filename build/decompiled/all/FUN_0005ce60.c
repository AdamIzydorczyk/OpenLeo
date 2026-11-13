
undefined4 * __regparm3 FUN_0005ce60(undefined4 param_1,undefined4 *param_2)

{
  bool bVar1;
  char *pcVar2;
  undefined **ppuVar3;
  
  pcVar2 = (char *)0x0;
  ppuVar3 = &PTR_FUN_000961fc;
  if (DAT_000a9aa8 < 2) {
    if (DAT_000a9aa8 == 1) {
      FUN_00057990(0,&PTR_FUN_000961fc);
      goto LAB_0005cee3;
    }
  }
  else {
    if (DAT_000a9aa8 < 3) {
      FUN_00057990(0,&PTR_FUN_000961fc);
      goto LAB_0005cee3;
    }
    if (DAT_000a9aa8 == 3) {
      FUN_00057990(0,&PTR_FUN_000961fc);
      goto LAB_0005cee3;
    }
  }
  ll_printf(&DAT_000a92d4,s_Nieznana_wersja_jezykowa__00092614);
  ll_rethrow_report();
LAB_0005cee3:
  if ((pcVar2 == (char *)0x0) || (*pcVar2 == '\0')) {
    bVar1 = true;
  }
  else {
    bVar1 = false;
  }
  if (bVar1) {
    *param_2 = 0;
    param_2[1] = &PTR_FUN_00096200;
    FUN_00057990(pcVar2,ppuVar3);
    param_2[1] = &PTR_FUN_000961fc;
  }
  else {
    FUN_0005c6b0();
    FUN_00057a70();
  }
  FUN_00057a00();
  return param_2;
}

