
int __regparm3 FUN_0004a4c0(undefined4 *param_1)

{
  int iVar1;
  undefined4 *puVar2;
  int iVar3;
  int iVar5;
  int iVar4;
  
  *param_1 = 0;
  param_1[1] = 0;
  param_1[3] = 0;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = &PTR_FUN_00098274;
  puVar2 = param_1 + 7;
  param_1[2] = 0;
  do {
    *puVar2 = 0;
    puVar2 = puVar2 + 1;
  } while (puVar2 != param_1 + 9);
  param_1[10] = 0;
  param_1[0xb] = &PTR_FUN_000961fc;
  param_1[0xc] = 0;
  param_1[0xd] = &PTR_FUN_000961fc;
  param_1[0xf] = &PTR_FUN_000961fc;
  param_1[0xe] = 0;
  param_1[0x10] = 0;
  param_1[0x1a] = 0;
  param_1[0x1b] = &PTR_FUN_000961fc;
  param_1[0xe3] = 0;
  param_1[0xe4] = 0;
  param_1[0xe5] = 0;
  param_1[0xe6] = 0;
  param_1[0xe9] = 1;
  param_1[0xea] = 0;
  param_1[0xeb] = 0;
  param_1[0xec] = 0;
  param_1[0xed] = 0;
  FUN_000217e0();
  iVar3 = FUN_000223d0();
  iVar1 = iVar3 + -0x3ce;
  *(undefined4 *)(iVar3 + 0x14) = 0;
  *(undefined ***)(iVar3 + 0x18) = &PTR_LAB_000a1350;
  iVar5 = iVar1;
  *(int *)(iVar3 + -0x32) = DAT_000cef40;
  DAT_000cef40 = iVar5;
  iVar5 = iVar1;
  do {
    iVar4 = iVar5 + 4;
    *(undefined4 *)(iVar5 + 0x58) = 0;
    *(undefined4 *)(iVar5 + 0x44) = 0;
    iVar5 = iVar4;
  } while (iVar4 != iVar3 + -0x3c2);
  *(undefined4 *)(iVar3 + -0x36a) = 0;
  *(undefined4 *)(iVar3 + -0x352) = 0;
  *(undefined4 *)(iVar3 + -0x34e) = 0;
  *(undefined4 *)(iVar3 + -0x34a) = 0;
  *(undefined4 *)(iVar3 + -0x2e) = 0;
  *(undefined4 *)(iVar3 + -0x35a) = 0;
  *(undefined4 *)(iVar3 + -0x35e) = 0;
  *(undefined4 *)(iVar3 + -0x356) = 0;
  FUN_0004a8c0();
  FUN_0004c8a0();
  return iVar1;
}

