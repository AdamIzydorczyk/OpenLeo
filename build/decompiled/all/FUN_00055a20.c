
void __regparm3 FUN_00055a20(undefined4 *param_1,int param_2)

{
  *param_1 = 0;
  param_1[1] = *(undefined4 *)(param_2 + 4);
  param_1[2] = *(undefined4 *)(param_2 + 8);
  param_1[3] = *(undefined4 *)(param_2 + 0xc);
  param_1[4] = 0;
  param_1[5] = &PTR_FUN_00096200;
  FUN_00057990();
  param_1[5] = &PTR_FUN_000961fc;
  param_1[6] = &PTR_LAB_000a2744;
  return;
}

