
undefined4 __regparm3 FUN_0001034c(undefined4 param_1,byte param_2)

{
  uint extraout_ECX;
  undefined4 local_18;
  undefined *local_14;
  undefined4 local_10;
  
  if ((param_2 & 4) == 0) {
    local_18 = DAT_000cf4f4;
    local_14 = &DAT_00096138;
    DAT_000cf4f4 = &local_18;
    local_10 = 0;
    param_1 = FUN_00057a00(param_1,param_2);
    if ((extraout_ECX & 2) != 0) {
      FUN_0005c370();
    }
    DAT_000cf4f4 = (undefined4 *)local_18;
  }
  else {
    FUN_00074bc0();
    thunk_FUN_0005c370();
  }
  return param_1;
}

