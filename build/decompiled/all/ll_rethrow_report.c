
/* sets [0xceedc]; Second throw! path */

void __regparm3 ll_rethrow_report(undefined4 *param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined4 *extraout_EDX;
  
  if (DAT_000ceedc != (undefined4 *)0x0) {
    uVar1 = (**(code **)*param_1)(param_2);
    FUN_000751b8(s_Second_throw___s_00091c68,uVar1);
    FUN_00076dc1();
    param_1 = extraout_EDX;
  }
  DAT_000ceedc = param_1;
  FUN_00076e0f();
  return;
}

