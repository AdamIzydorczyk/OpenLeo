
int __regparm3 FUN_00077010(byte *param_1,byte *param_2)

{
  int unaff_EBX;
  
  while( true ) {
    if (unaff_EBX == 0) {
      return 0;
    }
    if (*param_1 != *param_2) break;
    if (*param_1 == 0) {
      return 0;
    }
    param_1 = param_1 + 1;
    param_2 = param_2 + 1;
    unaff_EBX = unaff_EBX + -1;
  }
  return (uint)*param_1 - (uint)*param_2;
}

