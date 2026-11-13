
void __regparm3 FUN_00059b40(int param_1,int param_2)

{
  int extraout_ECX;
  
  if ((param_2 != *(int *)(param_1 + 0x28)) || (*(int *)(param_1 + 0x2c) != 2)) {
    FUN_000765ab();
    *(undefined4 *)(extraout_ECX + 0x2c) = 2;
    *(int *)(extraout_ECX + 0x28) = param_2;
  }
  return;
}

