
void __regparm3 FUN_00049b20(int param_1)

{
  int iVar1;
  undefined4 *puVar2;
  
  FUN_00049ba0();
  iVar1 = (**(code **)(*DAT_000cef14 + 0xc))();
  *(int *)(param_1 + 1) = iVar1;
  if (iVar1 == 0) {
    puVar2 = (undefined4 *)FUN_00049700();
    *puVar2 = &PTR_LAB_000a11fc;
    ll_rethrow_report();
  }
  FUN_00057990();
  return;
}

