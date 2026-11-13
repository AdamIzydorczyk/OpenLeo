
void __regparm3 FUN_00053770(undefined4 param_1,int param_2)

{
  char extraout_CL;
  uint auStack_1c [3];
  undefined1 local_10;
  
  if (*(int *)(param_2 + 4) == 0x10) {
    if (*(int *)(param_2 + 8) == 0x8138764) {
      local_10 = 0;
      FUN_000749a0();
      auStack_1c[(uint)(int)extraout_CL >> 5] =
           auStack_1c[(uint)(int)extraout_CL >> 5] | 1 << (extraout_CL % ' ' & 0x1fU);
      FUN_00049580();
    }
  }
  else {
    FUN_00052bb0();
  }
  return;
}

