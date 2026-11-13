
void __regparm3 FUN_00013938(uint param_1)

{
  byte bVar1;
  byte bVar2;
  
  bVar1 = (byte)param_1;
  bVar2 = bVar1 & 0xf;
  switch((int)(param_1 & 0xff) >> 4) {
  case 1:
    if (DAT_000aefec == 0) {
      *(short *)(DAT_000afb74 + 0x44) = *(short *)(DAT_000afb74 + 0x44) + (ushort)bVar2 * -4;
      return;
    }
    break;
  case 2:
    if (DAT_000aefec == 0) {
      *(short *)(DAT_000afb74 + 0x44) = *(short *)(DAT_000afb74 + 0x44) + (ushort)bVar2 * 4;
      return;
    }
    break;
  case 3:
    *(byte *)(DAT_000afb74 + 0x51) = bVar2;
    return;
  case 4:
    *(byte *)(DAT_000afb74 + 0x52) = *(byte *)(DAT_000afb74 + 0x52) & 0xf0;
    goto LAB_000139b6;
  case 6:
    if (DAT_000aefec == 0) {
      if ((param_1 & 0xf) == 0) {
        DAT_000aefe8 = DAT_000afb62 + -1;
        return;
      }
      if (DAT_000aefea == 0) {
        DAT_000aefea = bVar1 & 0xf;
      }
      else {
        DAT_000aefea = DAT_000aefea - 1;
      }
      if (DAT_000aefea != 0) {
        DAT_000afb62 = DAT_000aefe8;
        return;
      }
    }
    break;
  case 7:
    *(byte *)(DAT_000afb74 + 0x52) = *(byte *)(DAT_000afb74 + 0x52) & 0xf;
    bVar2 = bVar1 << 4;
LAB_000139b6:
    *(byte *)(DAT_000afb74 + 0x52) = *(byte *)(DAT_000afb74 + 0x52) | bVar2;
    return;
  case 8:
    if (DAT_000a732c != 0) {
      *(byte *)(DAT_000afb74 + 0x31) = bVar1 << 4;
      *(byte *)(DAT_000afb68 + 0x10d + DAT_000aefe4) = bVar1 << 4;
      return;
    }
    break;
  case 9:
    if ((param_1 & 0xf) != 0) {
      if (*(int *)(DAT_000afb74 + 0x3d) >> 0x18 == 0) {
        *(undefined1 *)(DAT_000afb74 + 0x29) = 1;
        *(byte *)(DAT_000afb74 + 0x40) = bVar2;
      }
      *(char *)(DAT_000afb74 + 0x40) = *(char *)(DAT_000afb74 + 0x40) + -1;
      return;
    }
    break;
  case 10:
    if ((DAT_000aefec == 0) &&
       (*(char *)(DAT_000afb74 + 0x43) = *(char *)(DAT_000afb74 + 0x43) + bVar2,
       0x40 < *(int *)(DAT_000afb74 + 0x40) >> 0x18)) {
      *(undefined1 *)(DAT_000afb74 + 0x43) = 0x40;
      return;
    }
    break;
  case 0xb:
    if ((DAT_000aefec == 0) &&
       (*(char *)(DAT_000afb74 + 0x43) = *(char *)(DAT_000afb74 + 0x43) - bVar2,
       *(int *)(DAT_000afb74 + 0x40) >> 0x18 < 0)) {
LAB_00013af8:
      *(undefined1 *)(DAT_000afb74 + 0x43) = 0;
      return;
    }
    break;
  case 0xc:
    if ((param_1 & 0xf) <= (uint)DAT_000aefec) goto LAB_00013af8;
    break;
  case 0xd:
    if ((uint)DAT_000aefec == (param_1 & 0xf)) {
      *(undefined1 *)(DAT_000afb74 + 0x29) = 1;
      return;
    }
    *(undefined1 *)(DAT_000afb74 + 0x29) = 0;
    return;
  case 0xe:
    if ((DAT_000aefec == 0) && (DAT_000aeff1 == '\0')) {
      DAT_000aeff0 = bVar2 + 1;
    }
  }
  return;
}

