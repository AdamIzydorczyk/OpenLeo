
void __regparm3 FUN_000555a0(int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  undefined1 local_38;
  undefined4 local_37;
  undefined4 local_33;
  undefined **local_2f;
  undefined4 local_2b;
  undefined4 local_27;
  undefined4 local_18;
  
  local_18 = param_2;
  FUN_0004a0b0();
  FUN_00057a40();
  local_37 = 0;
  local_33 = 0;
  local_2b = 0;
  local_27 = 0;
  local_2f = &PTR_FUN_000961fc;
  FUN_00049820();
  FUN_00057a00();
  FUN_00049920(&local_38,&DAT_000922cb,param_1 + 0x3c);
  uVar1 = thunk_FUN_0005c320();
  *(undefined4 *)(param_1 + 0x40) = uVar1;
  FUN_000749a0();
  iVar2 = 0;
  if (0 < *(int *)(param_1 + 0x3c)) {
    do {
      iVar2 = iVar2 + 1;
      (**(code **)(*(int *)(param_1 + 8) + 0x84))();
    } while (iVar2 < *(int *)(param_1 + 0x3c));
  }
  FUN_000529d0();
  FUN_00049a10();
  FUN_00057a00();
  return;
}

