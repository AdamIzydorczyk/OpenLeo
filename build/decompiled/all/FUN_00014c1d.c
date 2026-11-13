
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00014c1d(void)

{
  undefined2 uVar1;
  int iVar2;
  int *piVar3;
  short sVar4;
  int iVar5;
  int local_18;
  uint local_14;
  uint local_10;
  int *local_c;
  undefined2 local_8;
  
  if (DAT_000aeffc == 0) {
    if (DAT_000aeff8 == 0) {
      iVar2 = FUN_000151e8();
      uVar1 = DAT_000aefee;
      piVar3 = &local_18;
      if (iVar2 == 0) {
        DAT_000aefec = DAT_000aefec + 1;
        if (DAT_000afb66 <= DAT_000aefec) {
          DAT_000aefec = 0;
          sVar4 = (short)((_DAT_000afb62 & 0xffff) + 1);
          _DAT_000afb62 = CONCAT22(DAT_000afb64,sVar4);
          if (DAT_000aeff0 != '\0') {
            DAT_000aeff1 = DAT_000aeff0;
            DAT_000aeff0 = '\0';
          }
          if ((DAT_000aeff1 != '\0') && (DAT_000aeff1 = DAT_000aeff1 + -1, DAT_000aeff1 != '\0')) {
            DAT_000afb62 = sVar4 + -1;
          }
          if (DAT_000afb62 == DAT_000aeff2) {
            DAT_000aeff4 = 3;
          }
          if (DAT_000aeff4 != 0) {
            sVar4 = DAT_000aeff4 + -2;
            DAT_000aeff4 = 0;
            sVar4 = DAT_000afb64 + sVar4;
            _DAT_000afb62 = CONCAT22(sVar4,DAT_000aefee);
            DAT_000aefee = 0;
            if ((int)(uint)*(ushort *)(DAT_000aefe4 + 1) <= (int)sVar4) {
              if (DAT_000afb6c == 0) {
                DAT_000aefec = 0;
                DAT_000aefee = 0;
                DAT_000aeff4 = 0;
                return;
              }
              _DAT_000afb62 = CONCAT22(*(undefined2 *)(DAT_000aefe4 + 3),uVar1);
            }
            if ((int)_DAT_000afb62 >> 0x10 < 0) {
              _DAT_000afb62 = CONCAT22(*(short *)(DAT_000aefe4 + 1) + -1,DAT_000afb62);
            }
          }
          if (DAT_000aeff1 == '\0') {
            local_10 = 0;
            while ((int)local_10 < (int)(uint)*DAT_000aefe4) {
              DAT_000aeff2 = *(short *)(*(int *)(DAT_000aefe4 + 0x142) +
                                       (uint)DAT_000aefe4[((int)_DAT_000afb62 >> 0x10) + 0xd] * 2);
              DAT_000afb68 = (undefined2)local_10;
              DAT_000afb74 = (int *)((int)&DAT_000af000 + local_10 * 0x5b);
              if (*(ushort *)
                   (((uint)*DAT_000aefe4 * (uint)DAT_000aefe4[((int)_DAT_000afb62 >> 0x10) + 0xd] +
                    local_10) * 2 + *(int *)(DAT_000aefe4 + 0x13e)) < *(ushort *)(DAT_000aefe4 + 7))
              {
                iVar2 = FUN_00015274();
              }
              else {
                iVar2 = 0;
              }
              iVar5 = local_10 + 1;
              DAT_000afb74[0xf] = iVar2;
              FUN_00014328();
              local_10 = iVar5;
            }
          }
        }
        local_10 = 0;
        while ((int)local_10 < (int)(uint)*DAT_000aefe4) {
          DAT_000afb68 = (undefined2)local_10;
          iVar2 = local_10 * 0x5b;
          local_10 = local_10 + 1;
          DAT_000afb74 = (int *)((int)&DAT_000af000 + iVar2);
          FUN_000145bc();
        }
        local_c = &DAT_000af000;
        for (local_10 = 0; (int)local_10 < (int)(uint)*DAT_000aefe4; local_10 = local_10 + 1) {
          DAT_000afb74 = local_c;
          iVar2 = *local_c;
          iVar5 = local_c[1];
          if ((iVar2 != 0) && (iVar5 != 0)) {
            if (*(ushort *)(local_c + 0xd) < 0x28) {
              *(undefined2 *)(local_c + 0xd) = 0x28;
            }
            if (8000 < *(ushort *)(DAT_000afb74 + 0xd)) {
              *(undefined2 *)(DAT_000afb74 + 0xd) = 8000;
            }
            if (*(char *)((int)DAT_000afb74 + 0x29) != '\0') {
              local_18 = *(int *)((int)DAT_000afb74 + 0x29) >> 0x10;
              *(uint *)((int)piVar3 + -4) = (uint)*(ushort *)(iVar5 + 0x11);
              *(undefined4 *)((int)piVar3 + -8) = *(undefined4 *)(iVar5 + 0xd);
              local_14 = local_10 & 0xff;
              *(undefined4 *)((int)piVar3 + -0xc) = *(undefined4 *)(iVar5 + 9);
              *(undefined4 *)((int)piVar3 + -0x10) = 0x14faa;
              FUN_00012b14();
              *(undefined1 *)((int)DAT_000afb74 + 0x29) = 0;
              *(undefined1 *)(DAT_000afb74 + 10) = 1;
              *(undefined2 *)(DAT_000afb74 + 2) = 0x8000;
              *(int *)((int)piVar3 + -0x10) = iVar2 + 0x66;
              *(uint *)((int)piVar3 + -0x14) = (uint)*(byte *)(iVar2 + 0x65);
              *(uint *)((int)piVar3 + -0x18) = (uint)*(byte *)(iVar2 + 100);
              *(undefined4 *)((int)piVar3 + -0x1c) = 0x14ff3;
              FUN_00014aa4();
              *(int *)((int)piVar3 + -0x1c) = iVar2 + 0x9b;
              *(uint *)((int)piVar3 + -0x20) = (uint)*(byte *)(iVar2 + 0x9a);
              *(uint *)((int)piVar3 + -0x24) = (uint)*(byte *)(iVar2 + 0x99);
              *(undefined4 *)((int)piVar3 + -0x28) = 0x15031;
              FUN_00014aa4();
              piVar3 = (int *)((int)piVar3 + -0x24);
            }
            *(undefined4 *)((int)piVar3 + -4) = 0x15048;
            FUN_00014ad8();
            *(undefined4 *)((int)piVar3 + -4) = 0x15061;
            local_8 = FUN_00014ad8();
            *(undefined4 *)((int)piVar3 + -4) = 0x150b2;
            FUN_00012ad0();
            if ((*(byte *)(iVar5 + 0x11) & 0x40) != 0) {
              *(undefined4 *)((int)piVar3 + -4) = 0x150d6;
              FUN_00014a70();
            }
            *(undefined4 *)((int)piVar3 + -4) = 0x14e8b;
            FUN_00012afc();
            if ((DAT_000aefe4[0x12d] & 2) != 0) {
              *(undefined4 *)((int)piVar3 + -4) = 0x14eae;
              FUN_00014bc8();
            }
            *(undefined4 *)((int)piVar3 + -4) = 0x14ed2;
            FUN_00012ae8();
            if ((char)DAT_000afb74[10] == '\0') {
              if (*(ushort *)(DAT_000afb74 + 2) < *(ushort *)(iVar2 + 0xcf)) {
                *(undefined2 *)(DAT_000afb74 + 2) = 0;
              }
              else {
                *(ushort *)(DAT_000afb74 + 2) =
                     *(ushort *)(DAT_000afb74 + 2) - *(ushort *)(iVar2 + 0xcf);
              }
            }
          }
          local_c = (int *)((int)local_c + 0x5b);
        }
      }
    }
  }
  else {
    DAT_000aeffc = DAT_000aeffc + -1;
  }
  return;
}

