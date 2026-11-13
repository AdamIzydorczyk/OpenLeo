
int __regparm3 FUN_000137bc(byte param_1,ushort param_2)

{
  short sVar1;
  
  sVar1 = FUN_00013760((int)*(short *)(&DAT_000a796e +
                                      (uint)(ushort)((ushort)(param_1 % 0xc) * 8 +
                                                    (short)((int)(uint)param_2 >> 4)) * 2));
  return (int)sVar1 >> param_1 / 0xc;
}

