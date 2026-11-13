
void __regparm3 FUN_00013fa0(byte param_1)

{
  ushort uVar1;
  
  if (param_1 == 0) {
    param_1 = *(byte *)(DAT_000afb74 + 0x48);
  }
  else {
    *(ushort *)(DAT_000afb74 + 0x48) = (ushort)param_1;
  }
  if ((int)(uint)param_1 >> 4 == 0xf) {
    if (DAT_000aefec == 0) {
      uVar1 = (ushort)(param_1 & 0xf) << 2;
LAB_00013fed:
      *(short *)(DAT_000afb74 + 0x44) = *(short *)(DAT_000afb74 + 0x44) - uVar1;
      return;
    }
  }
  else if ((int)(uint)param_1 >> 4 == 0xe) {
    if (DAT_000aefec == 0) {
      uVar1 = param_1 & 0xf;
      goto LAB_00013fed;
    }
  }
  else if (DAT_000aefec != 0) {
    *(short *)(DAT_000afb74 + 0x44) = *(short *)(DAT_000afb74 + 0x44) + (ushort)param_1 * -4;
  }
  return;
}

