
void __regparm3 FUN_0003dd60(int param_1)

{
  int iVar1;
  undefined4 extraout_ECX;
  undefined4 local_1c;
  undefined4 local_18;
  undefined **local_14;
  undefined1 *local_10;
  
  FUN_00045120();
  local_10 = (undefined1 *)&local_1c;
  local_1c = **(undefined4 **)(param_1 + 0xf2);
  local_18 = (*(undefined4 **)(param_1 + 0xf2))[1];
  local_14 = &PTR_LAB_00098278;
  *(undefined4 *)(param_1 + 0xfa) = local_1c;
  *(undefined4 *)(param_1 + 0xfe) = local_18;
  iVar1 = FUN_000498f0(extraout_ECX,s_F____d___d_00091446,param_1 + 0x11a,param_1 + 0x11e);
  if (iVar1 != 2) {
    *(undefined4 *)(param_1 + 0x11e) = 0;
    *(undefined4 *)(param_1 + 0x11a) = *(undefined4 *)(param_1 + 0x11e);
  }
  return;
}

