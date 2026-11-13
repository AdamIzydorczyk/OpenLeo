
void __regparm3 FUN_00027c90(int param_1)

{
  int extraout_EDX;
  int extraout_EDX_00;
  longdouble lVar1;
  double dVar2;
  
  dVar2 = (double)*(int *)(param_1 + 0x1a2);
  lVar1 = (longdouble)FUN_00075d96(dVar2);
  if (((int)ROUND(lVar1) <= *(int *)(extraout_EDX + 0x13e)) ||
     (*(int *)(extraout_EDX + 0x146) <= (int)ROUND(lVar1))) {
    lVar1 = (longdouble)FUN_00075d96(dVar2,(double)*(int *)(extraout_EDX + 0x1a2));
    if (*(int *)(extraout_EDX_00 + 0x13e) <= (int)ROUND(lVar1)) {
      return;
    }
    if ((int)ROUND(lVar1) <= *(int *)(extraout_EDX_00 + 0x146)) {
      return;
    }
  }
  FUN_00054c00();
  return;
}

