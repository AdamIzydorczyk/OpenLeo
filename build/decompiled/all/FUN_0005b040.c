
/* WARNING: Switch with 1 destination removed at 0x0005b29b */

undefined4 __regparm3 FUN_0005b040(undefined4 param_1,undefined4 param_2,byte *param_3)

{
  int *piVar1;
  char *pcVar2;
  byte *pbVar3;
  undefined1 *puVar4;
  byte bVar5;
  char cVar6;
  byte bVar7;
  undefined4 uVar8;
  undefined4 *puVar9;
  undefined1 *puVar10;
  undefined1 *puVar12;
  undefined1 *puVar11;
  undefined1 *puVar13;
  uint uVar14;
  int iVar15;
  byte *pbVar16;
  undefined1 auStack_60 [44];
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_24;
  undefined4 local_18;
  
  local_18 = param_1;
  local_24 = param_2;
  local_34 = 0;
  local_30 = 0;
  puVar10 = auStack_60;
LAB_0005b05c:
  do {
    puVar13 = puVar10;
    bVar5 = *param_3;
    puVar13[0x4c] = bVar5;
    puVar10 = puVar13;
    if (bVar5 == 0) goto switchD_0005b148_caseD_2;
    pbVar3 = param_3 + 1;
    if (bVar5 == 0x25) {
      bVar5 = param_3[1];
      param_3 = param_3 + 2;
      puVar13[0x4c] = bVar5;
      pbVar3 = param_3;
      if (bVar5 != 0x25) {
        bVar5 = 0x20;
        uVar14 = 0xffffffff;
LAB_0005b11b:
        puVar13[0x50] = bVar5;
LAB_0005b11f:
        if ((puVar13[0x4c] & 0x80) != 0) goto switchD_0005b148_caseD_2;
        switch((&DAT_000a9714)[(char)puVar13[0x4c]]) {
        case 2:
          goto switchD_0005b148_caseD_2;
        case 4:
          bVar5 = *param_3;
          param_3 = param_3 + 1;
          puVar13[0x4c] = bVar5;
          puVar13[0x50] = puVar13[0x50] | 1;
          goto LAB_0005b11f;
        case 5:
          if ((int)uVar14 < 0) {
            iVar15 = (int)(char)puVar13[0x4c];
          }
          else {
            iVar15 = (int)(char)puVar13[0x4c] + uVar14 * 10;
          }
          uVar14 = iVar15 - 0x30;
          bVar5 = *param_3;
          param_3 = param_3 + 1;
          puVar13[0x4c] = bVar5;
          goto LAB_0005b11f;
        case 6:
          if ((puVar13[0x50] & 1) == 0) {
            puVar9 = (undefined4 *)**(int **)(puVar13 + 100);
            **(int **)(puVar13 + 100) = (int)(puVar9 + 1);
            *(undefined4 *)(puVar13 + 0x44) = *puVar9;
          }
          if ((int)uVar14 < 0) {
            uVar14 = 1;
          }
          if (uVar14 == 0) goto LAB_0005b47b;
          goto LAB_0005b44a;
        case 7:
        case 8:
          *(undefined4 *)(puVar13 + 0x40) = 10;
          break;
        case 9:
          *(undefined4 *)(puVar13 + 0x40) = 0;
          break;
        case 10:
          *(char **)(puVar13 + -4) = s_scanf_nie_moze_wczytywac_zmienne_00092410;
          *(undefined **)(puVar13 + -8) = &DAT_000a92d4;
          *(undefined4 *)(puVar13 + -0xc) = 0x5b351;
          ll_printf();
          *(undefined4 *)(puVar13 + -4) = 0x5b359;
          ll_rethrow_report();
          goto switchD_0005b148_caseD_f;
        case 0xb:
          goto switchD_0005b148_caseD_b;
        case 0xc:
          bVar5 = *param_3;
          param_3 = param_3 + 1;
          puVar13[0x4c] = bVar5;
          puVar13[0x50] = puVar13[0x50] | 2;
          goto LAB_0005b11f;
        case 0xd:
          bVar5 = *param_3;
          param_3 = param_3 + 1;
          puVar13[0x4c] = bVar5;
          puVar13[0x50] = puVar13[0x50] | 4;
          goto LAB_0005b11f;
        case 0xe:
          *(undefined4 *)(puVar13 + 0x40) = 8;
          break;
        case 0xf:
          goto switchD_0005b148_caseD_f;
        case 0x10:
          *(undefined4 *)(puVar13 + -4) = 0x5b4a0;
          FUN_000749a0();
          bVar5 = *param_3;
          *(undefined4 *)(puVar13 + 0x24) = 0;
          pbVar3 = param_3 + 1;
          if (bVar5 == 0x5e) {
            *(undefined4 *)(puVar13 + 0x24) = 1;
            pbVar3 = param_3 + 2;
            bVar5 = param_3[1];
          }
          goto LAB_0005b4bb;
        case 0x11:
          *(undefined4 *)(puVar13 + 0x38) = *(undefined4 *)(puVar13 + 0x30);
          goto LAB_0005b22a;
        case 0x12:
          *(undefined4 *)(puVar13 + 0x40) = 0x10;
          break;
        case 0x13:
          *(undefined1 **)(puVar13 + -4) = puVar13 + 0x34;
          *(undefined1 **)(puVar13 + -8) = puVar13 + 0x30;
          *(undefined4 *)(puVar13 + -0xc) = 8;
          *(undefined4 *)(puVar13 + -0x10) = 0x5b306;
          uVar8 = FUN_0005b620();
          puVar10 = puVar13 + -0xc;
          if (*(int *)(puVar13 + 0x28) < 1) goto switchD_0005b148_caseD_2;
          puVar10 = puVar13 + -0xc;
          if ((puVar13[0x44] & 1) == 0) {
            puVar9 = (undefined4 *)**(int **)(puVar13 + 0x58);
            iVar15 = *(int *)(puVar13 + 0x20);
            **(int **)(puVar13 + 0x58) = (int)(puVar9 + 1);
            puVar9 = (undefined4 *)*puVar9;
            *(int *)(puVar13 + 0x20) = iVar15 + 1;
            *puVar9 = uVar8;
            puVar10 = puVar13 + -0xc;
          }
          goto LAB_0005b05c;
        case 0x14:
          puVar13[0x50] = puVar13[0x50] & 0xdf;
          bVar5 = *param_3;
          param_3 = param_3 + 1;
          puVar13[0x4c] = bVar5;
          goto LAB_0005b11f;
        case 0x15:
          goto switchD_0005b148_caseD_15;
        }
        *(undefined1 **)(puVar13 + -4) = puVar13 + 0x34;
        *(undefined1 **)(puVar13 + -8) = puVar13 + 0x30;
        *(uint *)(puVar13 + -0xc) = uVar14 & 0x7fff;
        *(undefined4 *)(puVar13 + -0x10) = 0x5b214;
        uVar8 = FUN_0005b620();
        puVar11 = puVar13 + -0xc;
        puVar12 = puVar13 + -0xc;
        piVar1 = (int *)(puVar13 + 0x28);
        *(undefined4 *)(puVar13 + 0x2c) = uVar8;
        puVar13 = puVar13 + -0xc;
        if (*piVar1 < 0) goto LAB_0005b5f3;
        puVar10 = puVar12;
        puVar13 = puVar11;
        if (*piVar1 == 0) goto switchD_0005b148_caseD_2;
LAB_0005b22a:
        cVar6 = puVar13[0x4c];
        if ((('@' < cVar6) && (cVar6 < '[')) && (cVar6 != 'X')) {
          puVar13[0x50] = puVar13[0x50] | 4;
        }
        bVar5 = puVar13[0x50];
        puVar10 = puVar13;
        if ((bVar5 & 1) == 0) {
          if ((bVar5 & 4) == 0) {
            if ((bVar5 & 2) == 0) {
              puVar9 = (undefined4 *)**(int **)(puVar13 + 100);
              **(int **)(puVar13 + 100) = (int)(puVar9 + 1);
              puVar9 = (undefined4 *)*puVar9;
              goto LAB_0005b2cc;
            }
            puVar9 = (undefined4 *)**(int **)(puVar13 + 100);
            **(int **)(puVar13 + 100) = (int)(puVar9 + 1);
            *(short *)*puVar9 = (short)*(undefined4 *)(puVar13 + 0x38);
          }
          else {
            puVar9 = (undefined4 *)**(int **)(puVar13 + 100);
            **(int **)(puVar13 + 100) = (int)(puVar9 + 1);
            puVar9 = (undefined4 *)*puVar9;
LAB_0005b2cc:
            *puVar9 = *(undefined4 *)(puVar13 + 0x38);
          }
          if (puVar13[0x4c] != 'n') {
            *(int *)(puVar13 + 0x2c) = *(int *)(puVar13 + 0x2c) + 1;
          }
        }
        goto LAB_0005b05c;
      }
    }
    param_3 = pbVar3;
    *(int *)(puVar13 + 0x30) = *(int *)(puVar13 + 0x30) + 1;
    *(undefined4 *)(puVar13 + -4) = 0x5b08d;
    bVar5 = (**(code **)(puVar13 + 0x48))();
    if (bVar5 == 0xff) goto LAB_0005b5f3;
    if (((puVar13[0x4c] & 0x80) == 0) && ((&DAT_000a9714)[(char)puVar13[0x4c]] == '\x01')) {
      while (((bVar5 & 0x80) == 0 && ((&DAT_000a9714)[(char)bVar5] == '\x01'))) {
        *(int *)(puVar13 + 0x30) = *(int *)(puVar13 + 0x30) + 1;
        *(undefined4 *)(puVar13 + -4) = 0x5b0d4;
        bVar5 = (**(code **)(puVar13 + 0x48))();
        if (bVar5 == 0xff) goto LAB_0005b5f3;
      }
      *(undefined4 *)(puVar13 + -4) = 0x5b0eb;
      (**(code **)(puVar13 + 0x3c))();
      *(int *)(puVar13 + 0x30) = *(int *)(puVar13 + 0x30) + -1;
    }
    else if (bVar5 != puVar13[0x4c]) {
      *(undefined4 *)(puVar13 + -4) = 0x5b107;
      (**(code **)(puVar13 + 0x3c))();
      goto switchD_0005b148_caseD_2;
    }
  } while( true );
switchD_0005b148_caseD_15:
  puVar13[0x50] = puVar13[0x50] | 0x20;
  bVar5 = *param_3;
  param_3 = param_3 + 1;
  puVar13[0x4c] = bVar5;
  goto LAB_0005b11f;
LAB_0005b4bb:
  bVar7 = bVar5;
  pbVar16 = pbVar3;
  if (bVar7 == 0) goto switchD_0005b148_caseD_2;
  puVar13[(int)(char)bVar7 >> 3 & 0x1f] =
       puVar13[(int)(char)bVar7 >> 3 & 0x1f] | '\x01' << (bVar7 & 7);
  bVar5 = *pbVar16;
  param_3 = pbVar16 + 1;
  if (bVar5 != 0x5d) {
    pbVar3 = param_3;
    if (((bVar5 == 0x2d) && ((char)bVar7 < (char)*param_3)) && (*param_3 != 0x5d)) {
      bVar5 = pbVar16[1];
      while (bVar7 = bVar7 + 1, pbVar3 = pbVar16 + 2, (char)bVar7 < (char)bVar5) {
        puVar13[(int)(char)bVar7 >> 3 & 0x1f] =
             puVar13[(int)(char)bVar7 >> 3 & 0x1f] | '\x01' << (bVar7 & 7);
      }
    }
    goto LAB_0005b4bb;
  }
  if (uVar14 == 0xffffffff) {
    uVar14 = 0x7fff;
  }
  if ((puVar13[0x50] & 1) == 0) {
    puVar9 = (undefined4 *)**(int **)(puVar13 + 100);
    **(int **)(puVar13 + 100) = (int)(puVar9 + 1);
    *(undefined4 *)(puVar13 + 0x44) = *puVar9;
  }
  *(undefined4 *)(puVar13 + 0x28) = 0;
  bVar5 = 0x5d;
  while (uVar14 = uVar14 - 1, -1 < (int)uVar14) {
    *(int *)(puVar13 + 0x30) = *(int *)(puVar13 + 0x30) + 1;
    *(undefined4 *)(puVar13 + -4) = 0x5b551;
    bVar5 = (**(code **)(puVar13 + 0x48))();
    if (((char)bVar5 == -1) ||
       (*(int *)(puVar13 + 0x20) = (int)(char)puVar13[(int)(char)bVar5 >> 3 & 0x1f],
       (uint)((*(uint *)(puVar13 + 0x20) & 1 << (bVar5 & 7)) != 0) == *(uint *)(puVar13 + 0x24)))
    break;
    *(int *)(puVar13 + 0x28) = *(int *)(puVar13 + 0x28) + 1;
    if ((puVar13[0x50] & 1) == 0) {
      pbVar3 = *(byte **)(puVar13 + 0x44);
      *pbVar3 = bVar5;
      *(byte **)(puVar13 + 0x44) = pbVar3 + 1;
    }
  }
  if (-1 < (int)uVar14) {
    *(undefined4 *)(puVar13 + -4) = 0x5b5bc;
    (**(code **)(puVar13 + 0x3c))();
    *(int *)(puVar13 + 0x30) = *(int *)(puVar13 + 0x30) + -1;
  }
  if ((*(int *)(puVar13 + 0x28) != 0) && ((puVar13[0x50] & 1) == 0)) {
    puVar4 = *(undefined1 **)(puVar13 + 0x44);
    *puVar4 = 0;
    *(undefined1 **)(puVar13 + 0x44) = puVar4 + 1;
    *(int *)(puVar13 + 0x2c) = *(int *)(puVar13 + 0x2c) + 1;
  }
  if (bVar5 == 0xff) {
LAB_0005b5f3:
    *(undefined4 *)(puVar13 + -4) = 0x5b5fe;
    (**(code **)(puVar13 + 0x3c))();
    puVar10 = puVar13;
    if (*(int *)(puVar13 + 0x2c) == 0) {
      return 0xffffffff;
    }
switchD_0005b148_caseD_2:
    return *(undefined4 *)(puVar10 + 0x2c);
  }
  goto LAB_0005b05c;
switchD_0005b148_caseD_b:
  bVar7 = *param_3;
  param_3 = param_3 + 1;
  bVar5 = puVar13[0x50] | 8;
  puVar13[0x4c] = bVar7;
  goto LAB_0005b11b;
  while (((bVar5 & 0x80) == 0 && ((&DAT_000a9714)[(char)bVar5] == '\x01'))) {
switchD_0005b148_caseD_f:
    *(int *)(puVar13 + 0x30) = *(int *)(puVar13 + 0x30) + 1;
    *(undefined4 *)(puVar13 + -4) = 0x5b368;
    bVar5 = (**(code **)(puVar13 + 0x48))();
    if ((char)bVar5 == -1) goto LAB_0005b5f3;
  }
  if ((puVar13[0x50] & 1) == 0) {
    puVar9 = (undefined4 *)**(int **)(puVar13 + 100);
    **(int **)(puVar13 + 100) = (int)(puVar9 + 1);
    *(undefined4 *)(puVar13 + 0x44) = *puVar9;
    *(int *)(puVar13 + 0x2c) = *(int *)(puVar13 + 0x2c) + 1;
  }
  if (uVar14 == 0xffffffff) {
    uVar14 = 0x7fff;
  }
  do {
    if ((puVar13[0x50] & 1) == 0) {
      pbVar3 = *(byte **)(puVar13 + 0x44);
      *pbVar3 = bVar5;
      *(byte **)(puVar13 + 0x44) = pbVar3 + 1;
    }
    *(int *)(puVar13 + 0x30) = *(int *)(puVar13 + 0x30) + 1;
    *(undefined4 *)(puVar13 + -4) = 0x5b3d4;
    bVar5 = (**(code **)(puVar13 + 0x48))();
  } while (((((char)bVar5 != -1) && (bVar5 != 0)) &&
           (((bVar5 & 0x80) != 0 || ((&DAT_000a9714)[(char)bVar5] != '\x01')))) &&
          (uVar14 = uVar14 - 1, 0 < (int)uVar14));
  *(undefined4 *)(puVar13 + -4) = 0x5b400;
  (**(code **)(puVar13 + 0x3c))();
  *(int *)(puVar13 + 0x30) = *(int *)(puVar13 + 0x30) + -1;
  if ((puVar13[0x50] & 1) == 0) {
    **(undefined1 **)(puVar13 + 0x44) = 0;
  }
  goto LAB_0005b05c;
  while( true ) {
    if ((puVar13[0x50] & 1) == 0) {
      pcVar2 = *(char **)(puVar13 + 0x44);
      *pcVar2 = cVar6;
      *(char **)(puVar13 + 0x44) = pcVar2 + 1;
    }
    uVar14 = uVar14 - 1;
    if ((int)uVar14 < 1) break;
LAB_0005b44a:
    *(int *)(puVar13 + 0x30) = *(int *)(puVar13 + 0x30) + 1;
    *(undefined4 *)(puVar13 + -4) = 0x5b454;
    cVar6 = (**(code **)(puVar13 + 0x48))();
    if (cVar6 == -1) goto LAB_0005b5f3;
  }
LAB_0005b47b:
  if ((puVar13[0x50] & 1) == 0) {
    *(int *)(puVar13 + 0x2c) = *(int *)(puVar13 + 0x2c) + 1;
  }
  goto LAB_0005b05c;
}

