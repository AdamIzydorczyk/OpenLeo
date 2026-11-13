
void __regparm3 FUN_0004dc00(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  int extraout_ECX;
  int iVar3;
  undefined1 local_28;
  undefined4 local_27;
  undefined4 local_23;
  undefined **local_1f;
  undefined4 local_1b;
  undefined4 local_17;
  
  local_27 = 0;
  local_23 = 0;
  local_1b = 0;
  local_17 = 0;
  local_1f = &PTR_FUN_000961fc;
  FUN_00049820();
  FUN_00049920(&local_28,&DAT_00091f27,param_1 + 0x80);
  if (*(int *)(param_1 + 0x80) != 0) {
    thunk_FUN_0005c320();
    uVar1 = FUN_00076bb7();
    iVar3 = 0;
    *(undefined4 *)(param_1 + 0x74) = uVar1;
    if (0 < *(int *)(param_1 + 0x80)) {
      iVar2 = 0;
      do {
        iVar3 = iVar3 + 1;
        (**(code **)(*(int *)(*(int *)(param_1 + 0x74) + iVar2) + 0x20))();
        iVar2 = extraout_ECX + 0xc;
      } while (iVar3 < *(int *)(param_1 + 0x80));
    }
  }
  FUN_00049a10();
  FUN_00057a00();
  return;
}

