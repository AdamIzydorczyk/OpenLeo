
void __regparm3 FUN_00024d00(int param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  int iVar4;
  int extraout_ECX;
  
  puVar3 = (undefined4 *)(**(code **)(*(int *)(*(int *)(DAT_000cef40 + 0x394) + 8) + 0x68))();
  uVar1 = *puVar3;
  uVar2 = puVar3[1];
  *(undefined4 *)(param_1 + 0x10a) = *(undefined4 *)(param_1 + 0x106);
  iVar4 = FUN_00024b90(uVar1,uVar2,&PTR_LAB_00098278);
  *(int *)(param_1 + 0x106) = iVar4;
  if (iVar4 != *(int *)(param_1 + 0x10a)) {
    *(byte *)(extraout_ECX + 6) = *(byte *)(extraout_ECX + 6) | 0x40;
  }
  return;
}

