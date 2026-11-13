
void __regparm3 FUN_00057d30(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  undefined4 auStack_200 [125];
  
  puVar2 = &DAT_000a9510;
  puVar3 = auStack_200;
  for (iVar1 = 0x7d; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
  }
  switch(param_2) {
  case 0:
    FUN_00049920();
    break;
  case 1:
    FUN_000498f0();
    break;
  case 2:
    FUN_00049950();
    break;
  case 3:
    FUN_000499b0();
  }
  FUN_00057990();
  return;
}

