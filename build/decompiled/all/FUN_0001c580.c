
void __regparm3 FUN_0001c580(int param_1,uint param_2)

{
  undefined4 unaff_EBX;
  
  if ((param_2 & 0x200) == 0) {
    param_2 = (&DAT_000ce284)[param_2];
  }
  *(undefined4 *)(param_1 + 0x20a + (param_2 & 0x7f) * 4) = unaff_EBX;
  return;
}

