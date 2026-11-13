
void __regparm3
FUN_000164dd(byte param_1,undefined2 param_2,uint param_3,undefined4 param_4,undefined4 param_5,
            uint param_6,ushort param_7)

{
  uint uVar1;
  uint unaff_EBX;
  
  if (unaff_EBX < param_3) {
    (&DAT_000b33d1)[(uint)param_1 * 0x28] = 0;
    if (((param_7 & 0x10) != 0) && (param_3 < param_6)) {
      param_6 = param_3;
    }
    uVar1 = (uint)param_1;
    (&DAT_000b33d4)[uVar1 * 0x14] = param_2;
    (&DAT_000b33d6)[uVar1 * 10] = unaff_EBX;
    *(uint *)(&DAT_000b33da + uVar1 * 0x28) = param_3;
    (&DAT_000b33d2)[uVar1 * 0x14] = param_7;
    (&DAT_000b33d0)[uVar1 * 0x28] = 1;
    *(undefined4 *)(&DAT_000b33de + uVar1 * 0x28) = param_5;
    *(uint *)(&DAT_000b33e2 + uVar1 * 0x28) = param_6;
  }
  return;
}

