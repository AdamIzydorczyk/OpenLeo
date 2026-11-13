
int __regparm3 FUN_0007f7b4(int param_1,byte param_2,undefined4 param_3)

{
  uint extraout_ECX;
  undefined4 local_20;
  undefined *local_1c;
  undefined4 local_18;
  int local_14;
  byte local_10;
  undefined4 uStack_8;
  
  uStack_8 = param_3;
  if ((param_2 & 4) == 0) {
    local_1c = &DAT_000a6b4c;
    local_18 = 1;
    local_20 = DAT_000cf4f4;
    DAT_000cf4f4 = &local_20;
    *(undefined ***)(param_1 + 0x28) = &PTR_FUN_000a6b20;
    local_14 = param_1;
    local_10 = param_2;
    if (*(int *)(param_1 + 0x2c) != -1) {
      if (*(int *)(param_1 + 0x20) != *(int *)(param_1 + 0x18)) {
        (**(code **)(*(int *)(param_1 + 0x28) + 0x20))();
      }
      if ((*(byte *)(param_1 + 0x39) & 1) == 0) {
        FUN_00080fc0();
      }
    }
    local_18 = 0;
    param_1 = FUN_00080f58();
    if ((extraout_ECX & 2) != 0) {
      FUN_0005c370();
    }
    DAT_000cf4f4 = (undefined4 *)local_20;
  }
  else {
    FUN_00074bc0();
    thunk_FUN_0005c370();
  }
  return param_1;
}

