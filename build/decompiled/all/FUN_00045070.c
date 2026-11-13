
undefined4 * __regparm3 FUN_00045070(undefined4 *param_1)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_1c;
  undefined4 local_18;
  
  thunk_FUN_0005c320();
  uVar3 = FUN_00076bb7();
  param_1[2] = 1;
  *param_1 = &PTR_LAB_000a0cc0;
  param_1[1] = uVar3;
  (*(code *)PTR_FUN_0009827c)();
  puVar1 = (undefined4 *)param_1[1];
  *puVar1 = local_28;
  puVar1[1] = local_24;
  (*(code *)PTR_FUN_0009827c)();
  iVar2 = param_1[1];
  *(undefined4 *)(iVar2 + 0xd) = local_1c;
  *(undefined4 *)(iVar2 + 0x11) = local_18;
  *(undefined1 *)(param_1[1] + 0xc) = 1;
  *(undefined1 *)(param_1[1] + 0x19) = 1;
  return param_1;
}

