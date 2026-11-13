
void __regparm3 FUN_0005d9a0(undefined4 param_1,int param_2,undefined4 param_3)

{
  uint extraout_ECX;
  int extraout_EDX;
  
  param_2 = param_2 * 4;
  do {
    FUN_00050080();
    DAT_000cf468 = extraout_ECX;
    if (extraout_ECX < 0xb) {
                    /* WARNING: Could not recover jumptable at 0x0005d9c7. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (**(code **)(&DAT_0005d970 + param_2))(param_3);
      return;
    }
    if (extraout_EDX == 1) {
      param_2 = param_2 + 4;
    }
  } while (extraout_EDX != 0);
  return;
}

