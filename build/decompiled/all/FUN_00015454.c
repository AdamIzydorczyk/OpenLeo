
undefined4 __regparm3 FUN_00015454(char *param_1,char *param_2)

{
  char cVar1;
  char cVar2;
  ushort unaff_BX;
  ushort uVar3;
  
  uVar3 = 0;
  while( true ) {
    if (unaff_BX <= uVar3) {
      return 1;
    }
    cVar1 = *param_1;
    param_1 = param_1 + 1;
    cVar2 = *param_2;
    param_2 = param_2 + 1;
    if (cVar1 != cVar2) break;
    uVar3 = uVar3 + 1;
  }
  return 0;
}

