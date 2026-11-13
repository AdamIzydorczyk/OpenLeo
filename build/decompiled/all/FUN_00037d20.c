
void __regparm3 FUN_00037d20(int param_1,undefined4 param_2,undefined4 param_3)

{
  int extraout_ECX;
  longdouble lVar1;
  
  FUN_00037ce0((double)*(int *)(param_1 + 0x13a),param_2,param_3);
  lVar1 = (longdouble)FUN_00075d96();
  *(int *)(extraout_ECX + 4) = (int)ROUND(lVar1);
  return;
}

