
void __regparm3 FUN_00024c70(undefined4 param_1,int *param_2)

{
  undefined4 *puVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar3;
  
  puVar1 = (undefined4 *)(**(code **)(*(int *)(*(int *)(DAT_000cef40 + 0x394) + 8) + 0x68))();
  if (*param_2 == 3) {
    iVar2 = FUN_00024b90(*puVar1,puVar1[1],&PTR_LAB_00098278);
    iVar3 = extraout_ECX;
    if ((iVar2 != *(int *)(extraout_ECX + 0x10e)) && (DAT_000cec24 < 4)) {
      (**(code **)(*(int *)(extraout_ECX + 8) + 0xd8))();
      iVar3 = extraout_ECX_00;
    }
    *(int *)(iVar3 + 0x10e) = iVar2;
  }
  FUN_00054450();
  return;
}

