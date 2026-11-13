
int __regparm3 FUN_00013840(byte param_1,ushort param_2)

{
  int iVar1;
  
  if (param_2 == 0) {
    iVar1 = 0x1092;
  }
  else {
    iVar1 = ((int)((uint)*(ushort *)(&DAT_000a7332 + (uint)(param_1 % 0xc) * 2) * 0x20ab) >>
            param_1 / 0xc) / (int)(uint)param_2;
  }
  return iVar1;
}

