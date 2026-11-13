
undefined8 __regparm3 FUN_0001369c(undefined4 param_1,undefined4 param_2)

{
  byte extraout_CL;
  undefined4 extraout_EDX;
  undefined4 local_54;
  undefined *local_50;
  undefined4 local_4c;
  byte local_48;
  undefined1 local_44;
  undefined4 local_43;
  undefined4 local_3f;
  undefined **local_3b;
  undefined1 *local_34;
  byte local_30;
  undefined4 *local_28;
  undefined1 local_24;
  undefined1 *local_1c;
  undefined1 local_18;
  
  local_54 = DAT_000cf4f4;
  local_50 = &DAT_00096244;
  local_43 = 0;
  local_3f = 0;
  local_3b = &PTR_FUN_000961fc;
  local_1c = &local_44;
  local_48 = local_48 | 1;
  local_18 = 0;
  local_34 = &local_44;
  local_28 = &local_3f;
  local_30 = 0;
  local_4c = 3;
  local_24 = 0x10;
  DAT_000cf4f4 = &local_54;
  FUN_00049a40();
  local_48 = local_48 & 0xfe;
  local_4c = 1;
  local_30 = local_30 | extraout_CL;
  FUN_00013600();
  FUN_00074b86();
  DAT_000cf4f4 = (undefined4 *)local_54;
  return CONCAT44(param_2,extraout_EDX);
}

