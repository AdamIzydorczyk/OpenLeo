
void __regparm3 FUN_00045dc0(int param_1,char param_2,undefined4 *param_3)

{
  int *unaff_EBX;
  
  FUN_000458e0();
  if (param_2 == '\x06') {
    *unaff_EBX = *unaff_EBX + *(int *)(param_1 + 8);
    *param_3 = 6;
  }
  return;
}

