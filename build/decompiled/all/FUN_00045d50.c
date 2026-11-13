
void __regparm3 FUN_00045d50(int param_1,char param_2,int *param_3)

{
  int *unaff_EBX;
  
  FUN_000458e0();
  if (*(char *)(param_1 + 0x10) == param_2) {
    *unaff_EBX = *unaff_EBX + *(int *)(param_1 + 8);
    *param_3 = *param_3 + *(int *)(param_1 + 0xc);
  }
  return;
}

