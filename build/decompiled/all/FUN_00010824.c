
void __regparm3 FUN_00010824(undefined4 param_1,int param_2)

{
  uint uVar1;
  int extraout_ECX;
  uint unaff_EBX;
  
  do {
    FUN_00010798(param_2);
    unaff_EBX = unaff_EBX - extraout_ECX;
    param_2 = param_2 + extraout_ECX;
    if (unaff_EBX < 0x10000) {
      uVar1 = unaff_EBX;
    }
    else {
      uVar1 = 0x10000;
    }
  } while (uVar1 != 0);
  return;
}

