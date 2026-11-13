
int __regparm3 FUN_000517e0(undefined4 *param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int *extraout_ECX;
  int *extraout_ECX_00;
  int *piVar4;
  int iVar5;
  int iVar6;
  int local_20;
  undefined4 *local_1c;
  undefined4 *local_18;
  
  *param_1 = *param_2;
  param_1[1] = param_2[1];
  *(undefined2 *)(param_1 + 2) = *(undefined2 *)(param_2 + 2);
  *(undefined2 *)((int)param_1 + 10) = *(undefined2 *)((int)param_2 + 10);
  *(undefined2 *)(param_1 + 3) = *(undefined2 *)(param_2 + 3);
  *(undefined4 *)((int)param_1 + 0xe) = *(undefined4 *)((int)param_2 + 0xe);
  *(undefined4 *)((int)param_1 + 0x12) = *(undefined4 *)((int)param_2 + 0x12);
  *(undefined4 *)((int)param_1 + 0x16) = *(undefined4 *)((int)param_2 + 0x16);
  local_1c = param_1;
  FUN_00022190();
  iVar5 = FUN_00021750();
  local_1c = (undefined4 *)(iVar5 + -0x2e);
  iVar6 = 0;
  *(undefined ***)(iVar5 + 0x16) = &PTR_FUN_000a260c;
  for (iVar5 = 0; iVar5 < *(int *)((int)param_2 + 0x36); iVar5 = iVar5 + 1) {
    iVar2 = FUN_0005c320();
    iVar3 = 0;
    piVar4 = extraout_ECX;
    if (iVar2 != 0) {
      iVar2 = *(int *)(*(int *)((int)param_2 + 0x2e) + iVar6);
      iVar3 = FUN_00045780();
      *(undefined4 *)(iVar3 + 8) = *(undefined4 *)(iVar2 + 8);
      *(undefined4 *)(iVar3 + 0xc) = *(undefined4 *)(iVar2 + 0xc);
      *(undefined4 *)(iVar3 + 0x10) = *(undefined4 *)(iVar2 + 0x10);
      uVar1 = *(undefined4 *)(iVar2 + 0x14);
      *(undefined ***)(iVar3 + 4) = &PTR_LAB_000a2638;
      *(undefined4 *)(iVar3 + 0x14) = uVar1;
      piVar4 = extraout_ECX_00;
    }
    local_20 = iVar3;
    local_18 = &local_20;
    if (piVar4[2] == piVar4[3]) {
      local_18 = &local_20;
      FUN_00021e80();
    }
    iVar3 = piVar4[2];
    piVar4[2] = iVar3 + 1;
    iVar6 = iVar6 + 4;
    *(undefined4 *)(iVar3 * 4 + *piVar4) = *local_18;
  }
  return (int)local_1c;
}

