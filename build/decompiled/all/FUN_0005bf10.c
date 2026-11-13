
undefined8 __regparm3 FUN_0005bf10(undefined4 *param_1,undefined4 param_2)

{
  undefined2 uVar1;
  int iVar2;
  undefined4 extraout_ECX;
  undefined4 uVar3;
  byte local_14;
  
  uVar3 = 0;
  FUN_00057990(0,&PTR_FUN_00096200);
  iVar2 = FUN_0007607b(uVar3,extraout_ECX);
  uVar1 = DAT_000cf2be;
  if (iVar2 == 0) {
    iVar2 = FUN_0005be30();
    if (iVar2 == 0) {
      FUN_0005c220();
    }
  }
  else {
    FUN_0007612f();
    local_14 = (byte)uVar1;
    DAT_000cf254 = local_14 - 0x40;
    FUN_0007612f();
    FUN_00057990();
    FUN_00057ae0();
    FUN_0007630a();
  }
  *param_1 = 0;
  param_1[1] = &PTR_FUN_00096200;
  FUN_00057990();
  param_1[1] = &PTR_FUN_000961fc;
  FUN_00057a00();
  return CONCAT44(param_2,param_1);
}

