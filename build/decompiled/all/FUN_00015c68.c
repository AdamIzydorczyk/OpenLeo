
uint __regparm3 FUN_00015c68(int param_1,int param_2,ushort param_3)

{
  uint uVar1;
  int unaff_EBX;
  
  uVar1 = (param_2 - param_1) / unaff_EBX;
  param_1 = param_1 + uVar1 * unaff_EBX;
  if (unaff_EBX < 0) {
    for (; param_2 <= param_1; param_1 = param_1 + unaff_EBX) {
      uVar1 = uVar1 + 1;
    }
  }
  else {
    for (; param_1 <= param_2; param_1 = param_1 + unaff_EBX) {
      uVar1 = uVar1 + 1;
    }
  }
  if ((int)(uint)param_3 <= (int)uVar1) {
    uVar1 = (uint)param_3;
  }
  return uVar1;
}

