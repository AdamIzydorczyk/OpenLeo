
undefined2 * __regparm3 FUN_00041ef0(undefined2 *param_1)

{
  *param_1 = 0x7fff;
  param_1[2] = 0x7fff;
  param_1[4] = 0x7fff;
  *(undefined2 *)((int)param_1 + 0x15) = 0;
  *(undefined2 *)((int)param_1 + 0x17) = 0;
  *(undefined2 *)((int)param_1 + 0x19) = 0;
  *(undefined4 *)((int)param_1 + 0x1b) = 0;
  *(undefined4 *)((int)param_1 + 0x1f) = 0;
  *(undefined4 *)((int)param_1 + 0x23) = 0;
  *(undefined4 *)((int)param_1 + 0x27) = 0;
  *(undefined4 *)((int)param_1 + 0x2b) = 0;
  *(undefined4 *)((int)param_1 + 0x2f) = 0;
  *(undefined ***)((int)param_1 + 0x33) = &PTR_LAB_000a0c08;
  FUN_00041f60();
  return param_1;
}

