
void __regparm3 FUN_00037ba0(undefined4 *param_1)

{
  *param_1 = *(undefined4 *)((int)param_1 + 0x12a);
  param_1[1] = *(undefined4 *)((int)param_1 + 0x12e);
  (&DAT_000cedf4)[*(int *)((int)param_1 + 0x132)] = param_1;
  return;
}

