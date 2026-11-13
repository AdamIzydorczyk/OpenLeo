
void FUN_000145bc(void)

{
  char cVar1;
  undefined1 uVar2;
  byte bVar3;
  ushort uVar4;
  undefined1 extraout_CL;
  undefined1 extraout_CL_00;
  char extraout_CL_01;
  short extraout_CX;
  short extraout_CX_00;
  undefined2 extraout_CX_01;
  short extraout_CX_02;
  int extraout_ECX;
  int extraout_ECX_00;
  uint uVar5;
  
  if (*(int *)(DAT_000afb74 + 0x3c) != 0) {
    FUN_0001520c();
    *(undefined2 *)(DAT_000afb74 + 0x38) = 0;
    *(undefined2 *)(DAT_000afb74 + 0x3a) = 0;
LAB_0001477b:
    cVar1 = FUN_00015230();
    if (cVar1 != '\0') {
      switch(cVar1) {
      default:
        FUN_00015254();
        goto LAB_0001477b;
      case '\x03':
        FUN_00015230();
        FUN_000142a0();
        goto LAB_0001477b;
      case '\x04':
        bVar3 = FUN_00015230();
        if (bVar3 != 0) {
          *(ushort *)(DAT_000afb74 + 0x48) = (ushort)bVar3 << 2;
        }
        if (extraout_CX != DAT_000aefec) {
          *(short *)(DAT_000afb74 + 0x44) =
               *(short *)(DAT_000afb74 + 0x44) - *(short *)(DAT_000afb74 + 0x48);
        }
        goto LAB_0001477b;
      case '\x05':
        bVar3 = FUN_00015230();
        if (bVar3 != 0) {
          *(ushort *)(DAT_000afb74 + 0x48) = (ushort)bVar3 << 2;
        }
        if (extraout_CX_00 != DAT_000aefec) {
          *(short *)(DAT_000afb74 + 0x44) =
               *(short *)(DAT_000afb74 + 0x44) + *(short *)(DAT_000afb74 + 0x48);
        }
        goto LAB_0001477b;
      case '\x06':
        bVar3 = FUN_00015230();
        *(undefined1 *)(DAT_000afb74 + 0x29) = extraout_CL;
        if (bVar3 != 0) {
          *(ushort *)(DAT_000afb74 + 0x4a) = (ushort)bVar3;
          *(short *)(DAT_000afb74 + 0x4a) = *(short *)(DAT_000afb74 + 0x4a) << 2;
        }
        FUN_00014218();
        break;
      case '\a':
        bVar3 = FUN_00015230();
        uVar5 = (uint)CONCAT11(bVar3,bVar3);
        if ((bVar3 & 0xf) != 0) {
          uVar5 = CONCAT11(bVar3,bVar3) & 0xffffff0f;
          *(char *)(DAT_000afb74 + 0x55) = (char)uVar5;
        }
        if ((uVar5 & 0xf000) != 0) {
          *(char *)(DAT_000afb74 + 0x54) = (char)((int)(uint)((byte)(uVar5 >> 8) & 0xf0) >> 2);
        }
        FUN_00013b68();
        break;
      case '\b':
        FUN_00015230();
        *(undefined1 *)(DAT_000afb74 + 0x29) = extraout_CL_00;
        FUN_00014218();
        goto LAB_00014716;
      case '\t':
        FUN_00015230();
        FUN_00013b68();
LAB_00014716:
        FUN_00013ce4();
        break;
      case '\n':
        bVar3 = FUN_00015230();
        uVar5 = (uint)CONCAT11(bVar3,bVar3);
        if ((bVar3 & 0xf) != 0) {
          uVar5 = CONCAT11(bVar3,bVar3) & 0xffffff0f;
          *(char *)(DAT_000afb74 + 0x58) = (char)uVar5;
        }
        if ((uVar5 & 0xf000) != 0) {
          *(char *)(DAT_000afb74 + 0x57) = (char)((int)(uint)((byte)(uVar5 >> 8) & 0xf0) >> 2);
        }
        FUN_00013c14();
        goto LAB_00014770;
      case '\v':
        uVar2 = FUN_00015230();
        if (extraout_ECX != DAT_000a732c) {
          *(undefined1 *)(DAT_000afb74 + 0x31) = uVar2;
          *(undefined1 *)(DAT_000afb68 + 0x10d + DAT_000aefe4) = uVar2;
        }
        goto LAB_0001477b;
      case '\f':
        bVar3 = FUN_00015230();
        if (bVar3 != 0) {
          *(ushort *)(DAT_000afb74 + 0x59) = (ushort)bVar3 << 8;
        }
        *(uint *)(DAT_000afb74 + 0x2d) = (uint)*(ushort *)(DAT_000afb74 + 0x59);
        uVar5 = *(uint *)(*(int *)(DAT_000afb74 + 4) + 5);
        if (uVar5 < *(uint *)(DAT_000afb74 + 0x2d)) {
          *(uint *)(DAT_000afb74 + 0x2d) = uVar5;
        }
        goto LAB_0001477b;
      case '\r':
        FUN_00015230();
        FUN_00013ce4();
        goto LAB_0001477b;
      case '\x0e':
        uVar4 = FUN_00015230();
        if ((char)extraout_CX_01 == DAT_000aeff1) {
          DAT_000afb64 = (uVar4 & 0xff) - 1;
          DAT_000aeff4 = 3;
          DAT_000aefee = extraout_CX_01;
        }
        goto LAB_0001477b;
      case '\x0f':
        bVar3 = FUN_00015230();
        if (extraout_CX_02 == DAT_000aefec) {
          if (0x40 < bVar3) {
            bVar3 = 0x40;
          }
          *(byte *)(DAT_000afb74 + 0x43) = bVar3;
        }
        goto LAB_0001477b;
      case '\x10':
        uVar2 = FUN_00015230();
        if (extraout_CL_01 == DAT_000aeff1) {
          uVar4 = CONCAT11(uVar2,uVar2) & 0xf0ff;
          uVar5 = ((int)(uint)(byte)(uVar4 >> 8) >> 4) * 10 + (uint)((byte)uVar4 & 0xf);
          DAT_000aefee = (undefined2)uVar5;
          if (0x40 < uVar5) {
            DAT_000aefee = 0x40;
          }
          DAT_000aeff4 = 3;
        }
        goto LAB_0001477b;
      case '\x11':
        FUN_00015230();
        FUN_00013938();
        goto LAB_0001477b;
      case '\x12':
        bVar3 = FUN_00015230();
        if (((short)extraout_ECX_00 == DAT_000aefec) && ((char)extraout_ECX_00 == DAT_000aeff1)) {
          if ((extraout_ECX_00 == DAT_000a7328) || (bVar3 < 0x20)) {
            if (bVar3 != 0) {
              DAT_000afb66 = (ushort)bVar3;
              DAT_000aefec = (short)extraout_ECX_00;
            }
          }
          else {
            DAT_000afb60 = bVar3;
          }
        }
        goto LAB_0001477b;
      case '\x13':
        FUN_00015230();
        FUN_000141dc();
        goto LAB_0001477b;
      case '\x14':
        FUN_00015230();
        FUN_00013d44();
        goto LAB_0001477b;
      case '\x15':
        FUN_00015230();
        FUN_00013f18();
        goto LAB_0001477b;
      case '\x16':
        FUN_00015230();
        FUN_00013fa0();
        goto LAB_0001477b;
      case '\x17':
        FUN_00015230();
        FUN_00014028();
LAB_00014770:
        *(undefined2 *)(DAT_000afb74 + 0x3a) = 1;
        goto LAB_0001477b;
      case '\x18':
        FUN_00015230();
        FUN_000140ac();
        goto LAB_0001477b;
      case '\x19':
        FUN_00015230();
        FUN_000141fc();
        goto LAB_0001477b;
      case '\x1a':
        FUN_00015230();
        FUN_00013dd0();
        goto LAB_0001477b;
      case '\x1b':
        DAT_000a7331 = FUN_00015230();
        goto LAB_0001477b;
      case '\x1c':
        FUN_00015230();
        FUN_00013e38();
        goto LAB_0001477b;
      case '\x1d':
        goto switchD_00014a4a_caseD_1d;
      }
      *(undefined2 *)(DAT_000afb74 + 0x38) = 1;
      goto LAB_0001477b;
    }
    if (*(short *)(DAT_000afb74 + 0x38) == 0) {
      *(undefined2 *)(DAT_000afb74 + 0x34) = *(undefined2 *)(DAT_000afb74 + 0x44);
    }
    if (*(short *)(DAT_000afb74 + 0x3a) == 0) {
      *(undefined1 *)(DAT_000afb74 + 0x33) = *(undefined1 *)(DAT_000afb74 + 0x43);
    }
  }
  return;
switchD_00014a4a_caseD_1d:
  FUN_00015230();
  FUN_00013ea0();
  goto LAB_0001477b;
}

