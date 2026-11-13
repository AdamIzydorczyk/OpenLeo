
ulonglong __regparm3 FUN_0007b262(ushort *param_1,int *param_2,ushort *param_3)

{
  byte bVar1;
  uint *puVar2;
  ushort *puVar3;
  undefined4 *puVar4;
  byte *pbVar5;
  bool bVar6;
  uint uVar7;
  int iVar8;
  undefined1 *extraout_ECX;
  undefined1 *puVar9;
  ushort *extraout_ECX_00;
  undefined1 *extraout_ECX_01;
  undefined1 *extraout_ECX_02;
  ushort *extraout_ECX_03;
  undefined1 *extraout_ECX_04;
  undefined1 *extraout_ECX_05;
  undefined4 extraout_ECX_06;
  short extraout_DX;
  int unaff_EBX;
  uint unaff_EBP;
  short in_DS;
  bool bVar10;
  
  bVar6 = true;
  bVar1 = *(byte *)(unaff_EBX + 0x15);
  *(undefined4 *)(unaff_EBX + 0xc) = 0;
  if (bVar1 < 0x69) {
    if (0x57 < bVar1) {
      if (bVar1 < 0x59) goto LAB_0007b2b9;
      if (bVar1 == 100) goto LAB_0007b2fc;
    }
  }
  else {
    if (0x69 < bVar1) {
      if (bVar1 < 0x75) {
        bVar10 = bVar1 == 0x6f;
      }
      else {
        if (bVar1 < 0x76) goto LAB_0007b2b9;
        bVar10 = bVar1 == 0x78;
      }
      if (!bVar10) goto LAB_0007b306;
LAB_0007b2b9:
      if ((*(byte *)(unaff_EBX + 0x14) & 0x20) == 0) {
        if ((*(byte *)(unaff_EBX + 0x14) & 0x10) == 0) {
          puVar2 = (uint *)*param_2;
          *param_2 = (int)(puVar2 + 1);
          unaff_EBP = *puVar2;
        }
        else {
          puVar3 = (ushort *)*param_2;
          *param_2 = (int)(puVar3 + 2);
          unaff_EBP = (uint)*puVar3;
        }
      }
      else {
        puVar2 = (uint *)*param_2;
        *param_2 = (int)(puVar2 + 1);
        unaff_EBP = *puVar2;
      }
      *(byte *)(unaff_EBX + 0x14) = *(byte *)(unaff_EBX + 0x14) & 0xf9;
    }
LAB_0007b2fc:
    if (*(int *)(unaff_EBX + 8) != -1) {
      *(undefined1 *)(unaff_EBX + 0x16) = 0x20;
    }
  }
LAB_0007b306:
  bVar1 = *(byte *)(unaff_EBX + 0x15);
  if (bVar1 < 0x65) {
    if (bVar1 < 0x50) {
      if (bVar1 < 0x46) {
        bVar10 = bVar1 == 0x45;
      }
      else {
        if (bVar1 < 0x47) goto LAB_0007b3c1;
        bVar10 = bVar1 == 0x47;
      }
      if (!bVar10) goto LAB_0007b670;
      goto LAB_0007b3f3;
    }
    if (0x50 < bVar1) {
      if (bVar1 < 0x58) {
        if (bVar1 != 0x53) goto LAB_0007b670;
LAB_0007b49e:
        if ((*(byte *)(unaff_EBX + 0x14) & 0x80) == 0) {
          if ((*(byte *)(unaff_EBX + 0x14) & 0x40) == 0) {
            puVar4 = (undefined4 *)*param_2;
            *param_2 = (int)(puVar4 + 1);
            param_1 = (ushort *)*puVar4;
            if (param_1 == (ushort *)0x0) {
              param_1 = param_3;
            }
          }
          else {
            puVar4 = (undefined4 *)*param_2;
            *param_2 = (int)(puVar4 + 1);
            param_1 = (ushort *)*puVar4;
            if (param_1 == (ushort *)0x0) goto LAB_0007b4c7;
          }
        }
        else {
          puVar4 = (undefined4 *)*param_2;
          *param_2 = (int)(puVar4 + 2);
          param_1 = (ushort *)*puVar4;
          if ((param_1 == (ushort *)0x0) && (*(short *)(puVar4 + 1) == 0)) {
LAB_0007b4c7:
            param_1 = param_3;
          }
          else {
            in_DS = *(short *)(puVar4 + 1);
          }
        }
        bVar1 = *(byte *)(unaff_EBX + 0x14);
        bVar6 = false;
        *(byte *)(unaff_EBX + 0x14) = bVar1 & 0xf9;
        if (*(char *)(unaff_EBX + 0x15) == 'S') {
          if ((bVar1 & 0x20) == 0) {
            uVar7 = (uint)(byte)*param_1;
            param_1 = (ushort *)((int)param_1 + 1);
          }
          else {
            uVar7 = (uint)*param_1;
            param_1 = param_1 + 1;
          }
        }
        else {
          if ((bVar1 & 0x20) == 0) goto LAB_0007b443;
          uVar7 = FUN_0007b0e5();
          param_1 = extraout_ECX_03;
        }
      }
      else {
        if (bVar1 < 0x59) {
LAB_0007b567:
          if (((*(byte *)(unaff_EBX + 0x14) & 1) != 0) && (unaff_EBP != 0)) {
            *(undefined1 *)(unaff_EBX + 0x17) = 0x30;
            *(undefined1 *)(unaff_EBX + 0x19) = 0;
            *(undefined1 *)(unaff_EBX + 0x18) = *(undefined1 *)(unaff_EBX + 0x15);
          }
LAB_0007b58a:
          FUN_00079689();
          puVar9 = extraout_ECX_04;
          if (*(char *)(unaff_EBX + 0x15) == 'X') {
            FUN_0007b74b();
            puVar9 = extraout_ECX_05;
          }
          goto LAB_0007b427;
        }
        if (bVar1 < 99) goto LAB_0007b670;
        if (99 < bVar1) goto LAB_0007b44d;
        pbVar5 = (byte *)*param_2;
        *param_2 = (int)(pbVar5 + 4);
        bVar1 = *pbVar5;
        *(byte *)((int)param_1 + 1) = 0;
        *(byte *)param_1 = bVar1;
        *(undefined4 *)(unaff_EBX + 8) = 1;
        bVar6 = false;
        uVar7 = 1;
        *(byte *)(unaff_EBX + 0x14) = *(byte *)(unaff_EBX + 0x14) & 0xf9;
      }
      goto LAB_0007b699;
    }
LAB_0007b5ad:
    if (*(int *)(unaff_EBX + 4) == 0) {
      if ((*(byte *)(unaff_EBX + 0x14) & 0x80) == 0) {
        *(undefined4 *)(unaff_EBX + 4) = 8;
      }
      else {
        *(undefined4 *)(unaff_EBX + 4) = 0xd;
      }
    }
    *(byte *)(unaff_EBX + 0x14) = *(byte *)(unaff_EBX + 0x14) & 0xf9;
    iVar8 = *param_2;
    *param_2 = iVar8 + 4;
    if ((*(byte *)(unaff_EBX + 0x14) & 0x80) != 0) {
      *param_2 = iVar8 + 8;
      FUN_0007b108();
      *(byte *)(param_1 + 2) = 0x3a;
    }
    FUN_0007b108();
    if (*(char *)(unaff_EBX + 0x15) == 'P') {
      FUN_0007b74b();
    }
LAB_0007b443:
    uVar7 = FUN_0007b0c1();
    param_1 = extraout_ECX_00;
  }
  else {
    if (bVar1 < 0x66) {
LAB_0007b3f3:
      FUN_0007b25b();
      in_DS = extraout_DX;
      goto LAB_0007b443;
    }
    if (bVar1 < 0x6f) {
      if (bVar1 < 0x67) {
LAB_0007b3c1:
        if ((*(byte *)(unaff_EBX + 0x14) & 0x10) != 0) {
          *param_2 = *param_2 + 4;
          FUN_0007b166();
          goto LAB_0007b443;
        }
      }
      else if (0x67 < bVar1) {
        if (bVar1 == 0x69) {
LAB_0007b44d:
          if ((*(byte *)(unaff_EBX + 0x14) & 0x20) == 0) {
            if ((*(byte *)(unaff_EBX + 0x14) & 0x10) == 0) {
              *param_2 = *param_2 + 4;
              FUN_00076c9c();
              puVar9 = extraout_ECX_02;
              goto LAB_0007b427;
            }
            *param_2 = *param_2 + 4;
          }
          else {
            *param_2 = *param_2 + 4;
          }
          FUN_000796d3();
          puVar9 = extraout_ECX_01;
          goto LAB_0007b427;
        }
        goto LAB_0007b670;
      }
      goto LAB_0007b3f3;
    }
    if (bVar1 < 0x70) {
      if ((*(byte *)(unaff_EBX + 0x14) & 1) != 0) {
        *(byte *)param_1 = 0x30;
        param_1 = (ushort *)((int)param_1 + 1);
      }
      FUN_000796d3();
      puVar9 = extraout_ECX;
LAB_0007b427:
      if ((*(int *)(unaff_EBX + 8) == 0) && ((byte)*param_1 == 0x30)) {
        *puVar9 = 0;
      }
      goto LAB_0007b443;
    }
    if (bVar1 < 0x73) {
      if (bVar1 == 0x70) goto LAB_0007b5ad;
    }
    else {
      if (bVar1 < 0x74) goto LAB_0007b49e;
      if (0x74 < bVar1) {
        if (bVar1 < 0x76) goto LAB_0007b58a;
        if (bVar1 == 0x78) goto LAB_0007b567;
      }
    }
LAB_0007b670:
    *(undefined4 *)(unaff_EBX + 4) = 0;
    bVar1 = *(byte *)(unaff_EBX + 0x15);
    *(byte *)((int)param_1 + 1) = 0;
    *(byte *)param_1 = bVar1;
    *(undefined4 *)(unaff_EBX + 8) = 1;
    bVar6 = false;
    *(byte *)(unaff_EBX + 0x14) = *(byte *)(unaff_EBX + 0x14) & 0xf9;
    uVar7 = 1;
  }
LAB_0007b699:
  if (!bVar6) goto LAB_0007b6ee;
  if ((byte)*param_1 == 0x2d) {
    *(undefined1 *)(unaff_EBX + 0x17) = 0x2d;
    uVar7 = uVar7 - 1;
LAB_0007b6d7:
    *(undefined1 *)(unaff_EBX + 0x18) = 0;
  }
  else {
    if ((*(byte *)(unaff_EBX + 0x14) & 2) != 0) {
      *(undefined1 *)(unaff_EBX + 0x17) = 0x20;
      goto LAB_0007b6d7;
    }
    if ((*(byte *)(unaff_EBX + 0x14) & 4) != 0) {
      *(undefined1 *)(unaff_EBX + 0x17) = 0x2b;
      goto LAB_0007b6d7;
    }
  }
  if (*(int *)(unaff_EBX + 8) < (int)uVar7) {
    *(uint *)(unaff_EBX + 8) = uVar7;
  }
  else {
    *(uint *)(unaff_EBX + 0xc) = *(int *)(unaff_EBX + 8) - uVar7;
  }
LAB_0007b6ee:
  if (*(char *)(unaff_EBX + 0x16) == '*') {
    *(undefined1 *)(unaff_EBX + 0x17) = 0;
    *(byte *)(unaff_EBX + 0x14) = *(byte *)(unaff_EBX + 0x14) & 0xf9;
  }
  if (((*(int *)(unaff_EBX + 8) == -1) || ((int)uVar7 < *(int *)(unaff_EBX + 8))) &&
     (*(char *)(unaff_EBX + 0x15) != 'c')) {
    *(uint *)(unaff_EBX + 8) = uVar7;
  }
  iVar8 = FUN_0007b0c1();
  *(int *)(unaff_EBX + 4) =
       *(int *)(unaff_EBX + 4) - (iVar8 + *(int *)(unaff_EBX + 8) + *(int *)(unaff_EBX + 0xc));
  return (ulonglong)CONCAT24(in_DS,extraout_ECX_06);
}

