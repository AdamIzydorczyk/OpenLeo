
int __regparm3 FUN_0001b600(int param_1,undefined1 param_2)

{
  int iVar1;
  undefined4 local_2c;
  undefined *local_28;
  undefined4 local_24;
  int local_20;
  undefined1 local_1c;
  
  local_2c = DAT_000cf4f4;
  local_28 = &DAT_0009633c;
  local_24 = 1;
  local_20 = param_1;
  local_1c = param_2;
  if (*(int *)(param_1 + 0x9c) == 0) {
    DAT_000cf4f4 = &local_2c;
    FUN_00019ec0();
  }
  else {
    DAT_000cf4f4 = &local_2c;
    FUN_0001a8b0();
  }
  FUN_0005c370();
  if (DAT_000c41c0 != 0) {
    FUN_00075302();
  }
  if ((*(byte *)(param_1 + 0xa1) & 0x80) != 0) {
    FUN_0001b280();
  }
  if (*(int *)(param_1 + 0xb4) != 0) {
    FUN_00075302();
  }
  if (*(int *)(param_1 + 0x90) != 0) {
    FUN_0007630a();
  }
  local_24 = 0;
  iVar1 = FUN_00076450();
  DAT_000cf4f4 = (undefined4 *)local_2c;
  return iVar1 + -0xbc;
}

