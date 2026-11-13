
void __regparm3 FUN_0003dcb0(int param_1,int param_2)

{
  int extraout_ECX;
  
  if (*(int *)(param_2 + 4) == 1) {
    FUN_00045200();
    if (*(int *)(param_1 + 0x112) != 0) {
      (**(code **)(*(int *)(*(int *)(param_1 + 0x112) + 8) + 0x10))
                (0,1,*(undefined4 *)(extraout_ECX + 8),*(undefined4 *)(extraout_ECX + 0xc));
    }
  }
  FUN_00054560();
  return;
}

