
void __regparm3 FUN_0004db50(int param_1)

{
  undefined4 uVar1;
  int extraout_ECX;
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
  FUN_00049920(&local_28,&DAT_00091f24,param_1 + 0x7c);
  if (*(int *)(param_1 + 0x7c) != 0) {
    thunk_FUN_0005c320();
    uVar1 = FUN_00076bb7();
    *(undefined4 *)(param_1 + 0x70) = uVar1;
    if (0 < *(int *)(param_1 + 0x7c)) {
      do {
        FUN_000455e0();
      } while (extraout_ECX < *(int *)(param_1 + 0x7c));
    }
  }
  FUN_00049a10();
  FUN_00057a00();
  return;
}

