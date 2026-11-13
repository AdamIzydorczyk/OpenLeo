
uint __regparm3 FUN_00015aa8(uint param_1,uint param_2)

{
  param_2 = param_2 & 0xffff;
  return (param_1 % param_2 << 0xb) / param_2 | param_1 / param_2 << 0xb;
}

