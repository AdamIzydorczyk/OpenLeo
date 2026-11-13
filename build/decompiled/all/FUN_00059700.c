
/* WARNING: Removing unreachable block (ram,0x0005972d) */

void FUN_00059700(void)

{
  int iVar1;
  undefined4 in_ECX;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 uVar2;
  int extraout_EDX;
  
  iVar1 = FUN_00059690(in_ECX);
  uVar2 = extraout_ECX;
  if (iVar1 == 0) {
    ll_printf(&DAT_000a92d4,s_Blad_przy_zamykaniu_pliku_do_zap_000923a9);
    ll_rethrow_report();
    uVar2 = extraout_ECX_00;
    iVar1 = extraout_EDX;
  }
  *(undefined4 *)(iVar1 + 0x16) = uVar2;
  FUN_00057990();
  return;
}

