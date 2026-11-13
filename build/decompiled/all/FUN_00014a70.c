
int __regparm3 FUN_00014a70(short param_1,short param_2)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  
  uVar1 = (int)param_2 - 0x80;
  uVar2 = (int)uVar1 >> 0x1f;
  iVar3 = (0x80 - ((uVar1 ^ uVar2) - uVar2)) * (param_1 + -0x80);
  iVar4 = iVar3 >> 0x1f;
  return ((int)((iVar3 + iVar4 * -0x80) - (uint)(iVar4 << 6 < 0)) >> 7) + (int)param_2;
}

