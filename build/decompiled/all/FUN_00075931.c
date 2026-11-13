
uint __regparm3 FUN_00075931(int *param_1,int param_2,undefined4 param_3)

{
  uint uVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  
  if (param_2 == 0) {
    if ((*(byte *)(param_1 + 9) & 2) == 0) {
      *(char **)(param_1[2] + 0xc) = s_no_handler_active_to_catch_throw_00095dd0;
      FUN_00075384(param_3);
      param_2 = extraout_ECX_00;
    }
    else {
      FUN_00077bdc();
      param_2 = extraout_ECX;
    }
  }
  *param_1 = param_2;
  uVar1 = **(uint **)(param_2 + 4);
  if (uVar1 != 0) {
    if (uVar1 < 5) {
      return uVar1 & 0xffffff00;
    }
    FUN_0007539b();
  }
  uVar1 = FUN_00075773();
  return uVar1;
}

