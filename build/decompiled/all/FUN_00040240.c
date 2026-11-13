
void __regparm3 FUN_00040240(int param_1,undefined1 param_2,undefined4 param_3)

{
  int extraout_ECX;
  undefined1 unaff_BL;
  
  out((short)*(undefined4 *)(param_1 + 4),param_2);
  thunk_FUN_00078bd7(param_3);
  out((short)*(undefined4 *)(extraout_ECX + 8),unaff_BL);
  thunk_FUN_00078bd7();
  return;
}

