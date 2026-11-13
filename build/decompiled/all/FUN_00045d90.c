
void __regparm3 FUN_00045d90(int param_1,char param_2,undefined4 *param_3)

{
  undefined4 *unaff_EBX;
  
  FUN_000458e0();
  if (param_2 == '\x06') {
    *unaff_EBX = *(undefined4 *)(param_1 + 8);
    *param_3 = 6;
  }
  return;
}

