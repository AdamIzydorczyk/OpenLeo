
int __regparm3 FUN_00079461(int param_1)

{
  byte bVar1;
  byte *pbVar2;
  int iVar3;
  int extraout_ECX;
  int unaff_EBX;
  
  if (param_1 == 0) {
    return 1;
  }
  pbVar2 = (byte *)FUN_00079458();
  iVar3 = 0;
  if (((unaff_EBX != 0) && (bVar1 = *pbVar2, bVar1 != 0)) &&
     ((bVar1 < 2 || ((5 < bVar1 && (bVar1 < 8)))))) {
    iVar3 = 1;
  }
  if (iVar3 == 0) {
    iVar3 = FUN_00079458();
    if (iVar3 == extraout_ECX) {
      return 1;
    }
    iVar3 = 0;
  }
  return iVar3;
}

