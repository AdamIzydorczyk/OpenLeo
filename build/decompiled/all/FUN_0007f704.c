
undefined8 __regparm3 FUN_0007f704(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 local_10;
  undefined *local_c;
  undefined4 local_8;
  undefined4 uStack_4;
  
  local_c = &DAT_000a6b04;
  local_8 = 0;
  local_10 = DAT_000cf4f4;
  DAT_000cf4f4 = &local_10;
  uStack_4 = param_2;
  iVar1 = FUN_00080f74(param_1,0);
  *(undefined4 *)(iVar1 + 0x2c) = 0xffffffff;
  *(undefined ***)(iVar1 + 0x28) = &PTR_FUN_000a6b20;
  DAT_000cf4f4 = (undefined4 *)local_10;
  return CONCAT44(uStack_4,iVar1);
}

