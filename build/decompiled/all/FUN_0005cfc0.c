
int FUN_0005cfc0(void)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  
  DAT_000cde50 = 0;
  iVar1 = FUN_0005c320();
  iVar2 = 0;
  if (iVar1 != 0) {
    iVar2 = FUN_0001b2b0();
  }
  if (DAT_000cde50 == 3) {
    puVar3 = (undefined4 *)FUN_00049700();
    *puVar3 = &PTR_LAB_000a11fc;
    ll_rethrow_report();
  }
  if (DAT_000cde50 == 0) {
    FUN_0001b800();
  }
  iVar1 = DAT_000cde50;
  if (iVar2 != 0) {
    FUN_0001b600();
    FUN_0005c370();
  }
  return iVar1;
}

