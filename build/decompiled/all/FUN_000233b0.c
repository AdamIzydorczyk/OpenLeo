
int __regparm3 FUN_000233b0(undefined4 param_1,int param_2,undefined4 param_3)

{
  int extraout_ECX;
  
  *(undefined1 *)(param_2 + 0xc) = 0;
  FUN_000749a0(param_2,param_3);
  *(byte *)(extraout_ECX + 4) = *(byte *)(extraout_ECX + 4) | 0x80;
  return param_2;
}

