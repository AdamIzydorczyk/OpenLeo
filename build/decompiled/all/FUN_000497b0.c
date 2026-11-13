
void __regparm3 FUN_000497b0(int param_1)

{
  bool bVar1;
  int iVar2;
  undefined4 *puVar3;
  int extraout_ECX;
  
  FUN_00049a10();
  FUN_00057990();
  if ((*(char **)(param_1 + 5) == (char *)0x0) || (**(char **)(param_1 + 5) == '\0')) {
    bVar1 = true;
  }
  else {
    bVar1 = false;
  }
  if (!bVar1) {
    iVar2 = (**(code **)(*DAT_000cef14 + 0xc))();
    *(int *)(extraout_ECX + 1) = iVar2;
    if (iVar2 == 0) {
      puVar3 = (undefined4 *)FUN_00049700();
      *puVar3 = &PTR_LAB_000a11fc;
      ll_rethrow_report();
    }
  }
  return;
}

