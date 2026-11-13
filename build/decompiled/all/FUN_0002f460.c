
void __regparm3 FUN_0002f460(int param_1,undefined4 param_2,undefined4 param_3)

{
  int extraout_ECX;
  
  FUN_00054560(param_3);
  if (((*(int *)(extraout_ECX + 4) == 0x19) && (*(int *)(param_1 + 0xee) == 0)) &&
     (*(int *)(extraout_ECX + 8) == *(int *)(param_1 + 0xf2))) {
    *(undefined4 *)(param_1 + 0xee) = 1;
    *(byte *)(param_1 + 0x66) = *(byte *)(param_1 + 0x66) | 0x40;
  }
  return;
}

