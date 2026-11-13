
void FUN_00049a40(void)

{
  int iVar1;
  undefined4 *puVar2;
  int extraout_ECX;
  
  FUN_00049af0();
  FUN_00057990();
  iVar1 = (**(code **)(*DAT_000cef14 + 0xc))();
  *(int *)(extraout_ECX + 1) = iVar1;
  if (iVar1 == 0) {
    puVar2 = (undefined4 *)FUN_00049700();
    *puVar2 = &PTR_LAB_000a11fc;
    ll_rethrow_report();
  }
  return;
}

