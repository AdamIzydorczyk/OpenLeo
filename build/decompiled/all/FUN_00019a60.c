
/* WARNING: Instruction at (ram,0x00019e4a) overlaps instruction at (ram,0x00019e46)
    */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint __regparm3 FUN_00019a60(int param_1,int param_2)

{
  int *piVar1;
  short sVar2;
  byte bVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  char extraout_DL;
  char cVar7;
  undefined1 *puVar8;
  undefined1 *puVar9;
  undefined2 in_CS;
  undefined1 auStack_94 [128];
  
  DAT_000c4064 = FUN_00018a60();
  if (DAT_000c4064 == 0) {
    iVar5 = FUN_00019840();
    puVar9 = auStack_94 + 0x80;
    puVar8 = auStack_94 + 0x80;
    uVar4 = 0;
    if (iVar5 != 0) {
      _DAT_000c4098 = 0;
      _DAT_000c4094 = 0;
      _DAT_000c4090 = 0;
      _DAT_000c408c = 0;
      _DAT_000c40b0 = 0;
      DAT_000c40b4 = 0x5622;
      _DAT_000c40ac = 0;
      DAT_000c40b6 = 0x13b;
      if (((DAT_000c40b8 < 0x400) || (7 < DAT_000c40be)) || (DAT_000c40be < 4)) {
        DAT_000c409c = 0x80;
      }
      else {
        _DAT_000c40ac = 1;
        if (param_2 != 0) {
          _DAT_000c40b0 = 1;
          DAT_000c40b6 = 0x276;
          DAT_000c409c = 0;
        }
      }
      if ((0x2ff < DAT_000c40b8) && (_DAT_000c40ac = _DAT_000c40ac | 2, param_1 != 0)) {
        DAT_000c40b6 = DAT_000c40b6 * 2;
        _DAT_000c40b0 = _DAT_000c40b0 | 2;
      }
      if (0x1ff < DAT_000c40b8) {
        DAT_000c40b6 = DAT_000c40b6 * 2;
        _DAT_000c40ac = _DAT_000c40ac | 4;
      }
      if ((param_2 == 0) || ((_DAT_000c40ac & 1) != 0)) {
        if ((param_2 == 0) && ((param_1 != 0 && ((_DAT_000c40ac & 2) == 0)))) {
          _DAT_000c4098 = 1;
        }
      }
      else if ((param_1 == 0) || ((_DAT_000c40ac & 2) == 0)) {
        if ((param_1 == 0) || ((_DAT_000c40ac & 2) != 0)) {
          _DAT_000c4094 = 1;
        }
        else {
          _DAT_000c4090 = 1;
        }
      }
      else {
        _DAT_000c408c = 1;
      }
      if ((_DAT_000c40b0 & 1) == 0) {
        bVar3 = DAT_000c40bd;
      }
      else {
        bVar3 = DAT_000c40be;
        if (DAT_000c40be == 0xff) {
          DAT_000cde50 = 0xb;
          return 0;
        }
      }
      DAT_000c4070 = bVar3;
      if (DAT_000c40bc < 8) {
        DAT_000c40da = DAT_000c40bc + 8;
        _DAT_000c40ca = 0x20;
        DAT_000c40cc = 0x21;
      }
      else {
        DAT_000c40da = DAT_000c40bc + 0x68;
        DAT_000c40cc = 0xa1;
        _DAT_000c40ca = 0xa0;
      }
      DAT_000c40d8 = (ushort)(1 << DAT_000c40bc % 8);
      _DAT_000c40d6 = ~DAT_000c40d8;
      if ((_DAT_000c40b0 & 1) != 0) {
        DAT_000c40ce = 0xd4;
        DAT_000c4072 = 0xd8;
        sVar2 = (DAT_000c40be - 4) * 4;
        DAT_000c40d0 = 0xd6;
        DAT_000c4074 = sVar2 + 0xc2;
        DAT_000c40d2 = sVar2 + 0xc0;
        if (DAT_000c40be < 6) {
          if (DAT_000c40be == 5) {
            DAT_000c40d4 = 0x8b;
          }
        }
        else if (DAT_000c40be < 7) {
          DAT_000c40d4 = 0x89;
        }
        else if (DAT_000c40be == 7) {
          DAT_000c40d4 = 0x8a;
        }
        DAT_000c40dd = DAT_000c40be;
        DAT_000c40dc = DAT_000c40be - 4;
        if ((_DAT_000c40ac & 4) == 0) {
          DAT_000c40de = DAT_000c40be + 0x44;
        }
        else {
          DAT_000c40de = DAT_000c40be + 0x54;
        }
        DAT_000c4084 = DAT_000c40ba + 0xf;
        iVar5 = param_1;
        goto LAB_00019e60;
      }
      DAT_000c40d2 = (ushort)DAT_000c40bd * 2;
      DAT_000c40ce = 10;
      DAT_000c4074 = DAT_000c40d2 + 1;
      DAT_000c4072 = 0xc;
      DAT_000c40d0 = 0xb;
      switch(DAT_000c40bd) {
      case 0:
        DAT_000c40d4 = 0x87;
        break;
      case 1:
        DAT_000c40d4 = 0x83;
        break;
      case 2:
        DAT_000c40d4 = 0x81;
        break;
      case 3:
        DAT_000c40d4 = 0x82;
      }
      DAT_000340dd = DAT_000c40bd + 4;
      DAT_000340dc = DAT_000340bd;
      if ((DAT_000340ac & 4) == 0) {
        DAT_000c40de = DAT_000340bd + 'H';
      }
      else {
        DAT_000c40de = DAT_000340bd + 'X';
        puVar8 = auStack_94 + 0x80;
      }
      while( true ) {
        DAT_000c4084 = DAT_000c40ba + 0xe;
        iVar5 = param_1;
        puVar9 = puVar8;
LAB_00019e60:
        DAT_000c40e0 = DAT_000c40b6;
        *(undefined4 *)(puVar9 + -4) = 0x19e77;
        iVar6 = FUN_000196d0();
        if (iVar6 == 0) {
          return 0;
        }
        *(undefined4 *)(puVar9 + -4) = 0x19e80;
        FUN_00019f00();
        *(undefined4 *)(puVar9 + -4) = 0x19e8a;
        FUN_00075d5a();
        uVar4 = (uint)(param_2 != 0);
        piVar1 = (int *)((iVar5 != 0) + 0x1021f);
        *piVar1 = *piVar1 + uVar4;
        cVar7 = (char)iVar5 + extraout_DL;
        param_1 = CONCAT31((int3)((uint)iVar5 >> 8),cVar7);
        if ((iVar5 != 0) == 1 || cVar7 == '\0') break;
        *(undefined2 *)(puVar9 + -5) = in_CS;
        in_CS = 0xdea2;
        *(undefined4 *)(puVar9 + -9) = 0x19e52;
        cVar7 = func_0x48040001();
        puVar8 = puVar9 + -5;
        DAT_000c40de = cVar7 + '\x01';
      }
      *(bool *)(uVar4 + 0xc) = param_2 != 0;
      *(char *)(uVar4 + 1) = *(char *)(uVar4 + 1) + (char)((uint)iVar5 >> 8);
    }
  }
  else {
    uVar4 = 1;
    DAT_000c4064 = 1;
  }
  return uVar4;
}

