
void __regparm3 FUN_00057250(int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  undefined2 *puVar3;
  int iVar4;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  int unaff_EBX;
  undefined8 uVar5;
  int local_18;
  int local_14;
  int local_10;
  
  *(int *)(param_1 + 0x18) = unaff_EBX;
  *(int *)(param_1 + 0x1c) = unaff_EBX + -1;
  uVar1 = thunk_FUN_0005c320();
  *(undefined4 *)(param_1 + 0x44) = uVar1;
  uVar1 = thunk_FUN_0005c320();
  *(undefined4 *)(param_1 + 0x20) = uVar1;
  uVar1 = thunk_FUN_0005c320();
  *(undefined4 *)(param_1 + 0x24) = uVar1;
  local_18 = 0;
  uVar5 = thunk_FUN_0005c320();
  iVar4 = (int)((ulonglong)uVar5 >> 0x20);
  *(int *)(param_1 + 0x28) = (int)uVar5;
  if (0 < *(int *)(param_1 + 0x18)) {
    local_14 = 0;
    do {
      uVar1 = 0;
      FUN_00057990(0,&PTR_FUN_00096200,param_2,iVar4);
      *(undefined4 *)(*(int *)(param_1 + 0x44) + local_14) = 0;
      iVar2 = FUN_00057c60(uVar1,&PTR_FUN_000961fc);
      if (iVar2 == -1) {
        puVar3 = (undefined2 *)FUN_0005c320();
        uVar1 = 0;
        if (puVar3 != (undefined2 *)0x0) {
          *puVar3 = 0;
          puVar3[1] = 0;
          *(undefined ***)(puVar3 + 4) = &PTR_FUN_000a2790;
          *(undefined4 *)(puVar3 + 2) = 0;
          (*(code *)**(undefined4 **)(puVar3 + 4))();
          uVar1 = extraout_ECX_00;
        }
      }
      else {
        puVar3 = (undefined2 *)FUN_0005c320();
        uVar1 = 0;
        if (puVar3 != (undefined2 *)0x0) {
          *puVar3 = 0;
          puVar3[1] = 0;
          *(undefined4 *)(puVar3 + 2) = 0;
          *(undefined ***)(puVar3 + 4) = &PTR_LAB_000a27a4;
          puVar3[6] = 0;
          *(undefined4 *)(puVar3 + 7) = 0;
          *(undefined ***)(puVar3 + 4) = &PTR_FUN_000a277c;
          *(undefined4 *)(puVar3 + 9) = 0;
          (*(code *)**(undefined4 **)(puVar3 + 4))();
          uVar1 = extraout_ECX;
        }
      }
      local_18 = local_18 + 1;
      *(undefined4 *)(*(int *)(param_1 + 0x44) + local_14) = uVar1;
      FUN_00057a00();
      local_14 = local_14 + 4;
      iVar4 = iVar4 + 4;
    } while (local_18 < *(int *)(param_1 + 0x18));
  }
  local_10 = 0;
  if (0 < *(int *)(param_1 + 0x18)) {
    do {
      FUN_000574e0();
      FUN_000574d0();
      local_10 = local_10 + 1;
    } while (local_10 < *(int *)(param_1 + 0x18));
  }
  FUN_00057460();
  FUN_00057570((int)*(short *)(*(int *)(*(int *)(param_1 + 0x44) + *(int *)(param_1 + 0x1c) * 4) + 2
                              ));
  return;
}

