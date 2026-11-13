
void __regparm3 FUN_0001f6f0(int param_1,int param_2,undefined4 param_3)

{
  ushort uVar1;
  undefined4 *puVar2;
  
  puVar2 = (undefined4 *)(**(code **)(*(int *)(param_2 + 8) + 0x68))();
  uVar1 = FUN_0001f760(*puVar2,puVar2[1],&PTR_LAB_00098278,param_3);
  if (uVar1 < 2) {
    if (uVar1 != 1) goto LAB_0001f74e;
    *(byte *)(param_1 + 0x66) = *(byte *)(param_1 + 0x66) | 0x40;
LAB_0001f738:
    *(byte *)(param_1 + 0x66) = *(byte *)(param_1 + 0x66) | 0x80;
  }
  else {
    if (uVar1 < 3) goto LAB_0001f738;
    if (uVar1 != 3) goto LAB_0001f74e;
  }
  *(byte *)(param_1 + 0x67) = *(byte *)(param_1 + 0x67) | 1;
LAB_0001f74e:
  FUN_0003df50();
  return;
}

