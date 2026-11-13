
undefined8 __regparm3 FUN_00014bc8(int param_1,undefined4 param_2)

{
  return CONCAT44(param_2,(int)((uint)*(ushort *)(&DAT_000a736c + ((0x1e00 - param_1) % 0x300) * 2)
                               << 2) >> (7U - (char)((0x1e00 - param_1) / 0x300) & 0x1f));
}

