
int __regparm3 FUN_00080469(char *param_1,int param_2,int param_3)

{
  char cVar1;
  int iVar2;
  int unaff_EBX;
  int iVar3;
  int iVar4;
  
  if (param_2 < 0) {
    cVar1 = '-';
    param_2 = -param_2;
  }
  else {
    cVar1 = '+';
  }
  iVar3 = 100;
  iVar2 = 3;
  if (param_2 < 100) {
    iVar3 = 10;
    iVar2 = 2;
    if (param_2 < 10) {
      iVar2 = 1;
      iVar3 = 1;
    }
  }
  if ((unaff_EBX == 0) && (unaff_EBX = 2, iVar2 == 3)) {
    unaff_EBX = 3;
  }
  iVar4 = unaff_EBX + 1;
  if (iVar4 <= param_3) {
    if (unaff_EBX < iVar2) {
      iVar4 = iVar2 + 1;
    }
    else {
      *param_1 = cVar1;
      for (; param_1 = param_1 + 1, iVar2 < unaff_EBX; unaff_EBX = unaff_EBX + -1) {
        *param_1 = '0';
      }
      do {
        *param_1 = (char)(param_2 / iVar3) + '0';
        param_2 = param_2 % iVar3;
        iVar3 = iVar3 / 10;
        param_1 = param_1 + 1;
      } while (iVar3 != 0);
    }
  }
  return iVar4;
}

