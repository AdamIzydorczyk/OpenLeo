
int __regparm3 FUN_00013760(short param_1,short param_2,int param_3,short param_4)

{
  short unaff_BX;
  
  if (param_2 != unaff_BX) {
    param_3 = ((int)(short)(param_1 - param_2) * (int)(short)(param_4 - (short)param_3)) /
              (int)(short)(unaff_BX - param_2) + (int)(short)param_3;
  }
  return param_3;
}

