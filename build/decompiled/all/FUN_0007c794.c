
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0007c794(void)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined2 uVar3;
  short sVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  short extraout_CX;
  ushort uVar7;
  int iVar8;
  ushort uVar9;
  byte bVar10;
  uint uVar11;
  ushort *puVar12;
  uint uVar13;
  uint uVar14;
  ushort *puVar15;
  undefined4 *puVar16;
  uint uVar17;
  byte bVar18;
  undefined8 uVar19;
  byte *in_stack_00000038;
  
  bVar18 = 0;
code_r0x0007c794:
  bVar10 = *in_stack_00000038;
  while ((bVar10 < 0x9c &&
         ((((((bVar10 == 0x9b || (bVar10 == 0x26)) || (bVar10 == 0x2e)) ||
            ((bVar10 == 0x36 || (bVar10 == 0x3e)))) ||
           ((bVar10 == 100 || ((bVar10 == 0x65 || (bVar10 == 0x66)))))) || (bVar10 == 0x67))))) {
    in_stack_00000038 = in_stack_00000038 + 1;
    bVar10 = *in_stack_00000038;
  }
  if ((bVar10 & 0xf8) != 0xd8) {
    return;
  }
  bVar10 = in_stack_00000038[1];
  in_stack_00000038 = in_stack_00000038 + 2;
  if (0xbf < bVar10) {
    sVar4 = FUN_00074fb8();
    bVar10 = (byte)((ushort)sVar4 >> 8);
    uVar17 = (_DAT_000aafb4 & 0x3800) >> 0xb;
    uVar13 = bVar10 & 7;
    uVar11 = uVar13;
    if (sVar4 == -0x7ff) {
      uVar11 = 1;
    }
    uVar14 = uVar17 * 10;
    puVar12 = (ushort *)((uVar11 + uVar17 & 7) * 10);
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
    switch(bVar10 & 0x38 | (byte)sVar4) {
    case 0:
      goto switchD_0007c812_caseD_40;
    case 1:
      goto switchD_0007c812_caseD_41;
    default:
      goto switchD_0007c812_caseD_42;
    case 4:
      goto switchD_0007c812_caseD_44;
    case 5:
    case 7:
      goto switchD_0007c812_caseD_45;
    case 6:
      goto switchD_0007c812_caseD_46;
    case 8:
      goto switchD_0007c812_caseD_48;
    case 9:
    case 0xd:
    case 0xf:
      goto switchD_0007c812_caseD_49;
    case 0xc:
      goto switchD_0007c812_caseD_4c;
    case 0xe:
      goto switchD_0007c812_caseD_4e;
    case 0x10:
    case 0x14:
      goto switchD_0007c812_caseD_50;
    case 0x11:
      goto switchD_0007c812_caseD_51;
    case 0x15:
    case 0x17:
      goto switchD_0007c812_caseD_55;
    case 0x18:
    case 0x1c:
      goto switchD_0007c812_caseD_58;
    case 0x19:
    case 0x1d:
    case 0x1f:
      goto switchD_0007c812_caseD_59;
    case 0x1e:
      goto switchD_0007c812_caseD_5e;
    case 0x20:
      goto switchD_0007c812_caseD_60;
    case 0x21:
      goto switchD_0007c812_caseD_61;
    case 0x23:
      goto switchD_0007c812_caseD_63;
    case 0x24:
      goto switchD_0007c812_caseD_64;
    case 0x25:
      goto switchD_0007c812_caseD_65;
    case 0x26:
      goto switchD_0007c812_caseD_66;
    case 0x27:
      goto switchD_0007c812_caseD_67;
    case 0x28:
      goto switchD_0007c812_caseD_68;
    case 0x29:
      goto switchD_0007c812_caseD_69;
    case 0x2a:
      goto switchD_0007c812_caseD_6a;
    case 0x2c:
      goto switchD_0007c812_caseD_6c;
    case 0x2d:
      goto switchD_0007c812_caseD_6d;
    case 0x2e:
      goto switchD_0007c812_caseD_6e;
    case 0x30:
      goto switchD_0007c812_caseD_70;
    case 0x31:
      goto switchD_0007c812_caseD_71;
    case 0x34:
      goto switchD_0007c812_caseD_74;
    case 0x36:
      goto switchD_0007c812_caseD_76;
    case 0x38:
      goto switchD_0007c812_caseD_78;
    case 0x39:
      goto switchD_0007c812_caseD_79;
    case 0x3c:
      goto switchD_0007c812_caseD_7c;
    case 0x3e:
      goto switchD_0007c812_caseD_7e;
    }
  }
  uVar11 = CONCAT11(bVar10 >> 3,bVar10) & 0x1807;
  puVar12 = (ushort *)(uint)(byte)((byte)uVar11 | (byte)(uVar11 >> 8));
  uVar3 = (*(code *)(&PTR_LAB_0007c440)[(int)puVar12])();
  bVar10 = (byte)((ushort)uVar3 >> 8) & 0x38 | (byte)uVar3;
  uVar13 = (uint)bVar10;
  FUN_00074fb8();
  uVar14 = ((_DAT_000aafb4 & 0x3800) >> 0xb) * 10;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
  switch(bVar10) {
  case 0:
    FUN_0007e6e2();
    FUN_0007deb8();
                    /* WARNING: Could not recover jumptable at 0x0007cb0c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 1:
    _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
    _DAT_000aafb4 =
         CONCAT22(DAT_000aafb4_2,
                  DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c394 + uVar14)));
    DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + *(ushort *)(&DAT_0007c394 + uVar14));
    FUN_0007e6e2();
    FUN_0007c794();
    return;
  case 2:
    FUN_0007e4d6();
    FUN_0007deb8();
                    /* WARNING: Could not recover jumptable at 0x0007ccda. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 3:
    _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
    _DAT_000aafb4 =
         CONCAT22(DAT_000aafb4_2,
                  DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c394 + uVar14)));
    DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + *(ushort *)(&DAT_0007c394 + uVar14));
    FUN_0007e4d6();
    FUN_0007c794();
    return;
  case 4:
    FUN_0007e51c();
    FUN_0007deb8();
                    /* WARNING: Could not recover jumptable at 0x0007ce6a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 5:
    _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
    _DAT_000aafb4 =
         CONCAT22(DAT_000aafb4_2,
                  DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c394 + uVar14)));
    DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + *(ushort *)(&DAT_0007c394 + uVar14));
    FUN_0007e51c();
    FUN_0007c794();
    return;
  case 6:
    FUN_0007e4d6();
    FUN_0007deb8();
                    /* WARNING: Could not recover jumptable at 0x0007d0c7. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 7:
    _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
    _DAT_000aafb4 =
         CONCAT22(DAT_000aafb4_2,
                  DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c394 + uVar14)));
    DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + *(ushort *)(&DAT_0007c394 + uVar14));
    FUN_0007e4d6();
    FUN_0007c794();
    return;
  case 8:
    FUN_0007e6e2();
    FUN_0007e2ec();
                    /* WARNING: Could not recover jumptable at 0x0007cb2b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 9:
  case 0xb:
  case 0xd:
  case 0xf:
  case 0x23:
  case 0x2d:
  case 0x33:
    FUN_0007c794();
    return;
  case 10:
    FUN_0007e4d6();
    FUN_0007e2ec();
                    /* WARNING: Could not recover jumptable at 0x0007ccf9. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0xc:
    FUN_0007e51c();
    FUN_0007e2ec();
                    /* WARNING: Could not recover jumptable at 0x0007ce8d. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0xe:
    FUN_0007e4d6();
    FUN_0007e2ec();
                    /* WARNING: Could not recover jumptable at 0x0007d0e8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x10:
    FUN_0007e6e2();
    FUN_0007e07e();
    FUN_0007c988();
    return;
  case 0x11:
    uVar6 = FUN_0007e59c();
    *(undefined4 *)puVar12 = uVar6;
    FUN_0007c794();
    return;
  case 0x12:
    FUN_0007e4d6();
    FUN_0007e07e();
    FUN_0007c988();
    return;
  case 0x13:
    uVar6 = *(undefined4 *)((int)&DAT_000aafca + uVar14 + 2);
    uVar1 = *(undefined4 *)(&DAT_000aafd0 + uVar14);
    uVar2 = *(undefined4 *)(&DAT_000aafd2 + uVar14);
    FUN_0007e7a7();
    uVar5 = FUN_0007e467();
    *(undefined4 *)puVar12 = uVar5;
    *(undefined4 *)(&DAT_000aafd2 + uVar14) = uVar2;
    *(undefined4 *)(&DAT_000aafd0 + uVar14) = uVar1;
    *(undefined4 *)((int)&DAT_000aafca + uVar14 + 2) = uVar6;
    FUN_0007c794();
    return;
  case 0x14:
    FUN_0007e51c();
    FUN_0007e07e();
    FUN_0007c988();
    return;
  case 0x15:
    uVar19 = FUN_0007e61b();
    *(undefined8 *)puVar12 = uVar19;
    FUN_0007c794();
    return;
  case 0x16:
    FUN_0007e4d6();
    FUN_0007e07e();
    FUN_0007c988();
    return;
  case 0x17:
    uVar6 = *(undefined4 *)((int)&DAT_000aafca + uVar14 + 2);
    uVar1 = *(undefined4 *)(&DAT_000aafd0 + uVar14);
    uVar2 = *(undefined4 *)(&DAT_000aafd2 + uVar14);
    FUN_0007e7a7();
    iVar8 = FUN_0007e467();
    uVar7 = (ushort)iVar8;
    if ((short)uVar7 != iVar8) {
      uVar7 = 0x8000;
    }
    *puVar12 = uVar7;
    *(undefined4 *)(&DAT_000aafd2 + uVar14) = uVar2;
    *(undefined4 *)(&DAT_000aafd0 + uVar14) = uVar1;
    *(undefined4 *)((int)&DAT_000aafca + uVar14 + 2) = uVar6;
    FUN_0007c794();
    return;
  case 0x18:
    FUN_0007e6e2();
    FUN_0007e07e();
    FUN_0007c936();
    return;
  case 0x19:
    uVar6 = FUN_0007e59c();
    *(undefined4 *)puVar12 = uVar6;
    break;
  case 0x1a:
    FUN_0007e4d6();
    FUN_0007e07e();
    FUN_0007c936();
    return;
  case 0x1b:
    FUN_0007e7a7();
    uVar6 = FUN_0007e467();
    *(undefined4 *)puVar12 = uVar6;
    break;
  case 0x1c:
    FUN_0007e51c();
    FUN_0007e07e();
    FUN_0007c936();
    return;
  case 0x1d:
    uVar19 = FUN_0007e61b();
    *(undefined8 *)puVar12 = uVar19;
    break;
  case 0x1e:
    FUN_0007e4d6();
    FUN_0007e07e();
    FUN_0007c936();
    return;
  case 0x1f:
    FUN_0007e7a7();
    iVar8 = FUN_0007e467();
    uVar7 = (ushort)iVar8;
    if ((short)uVar7 != iVar8) {
      uVar7 = 0x8000;
    }
    *puVar12 = uVar7;
    break;
  case 0x20:
    FUN_0007e6e2();
    FUN_0007deaa();
                    /* WARNING: Could not recover jumptable at 0x0007cb82. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x21:
    puVar16 = (undefined4 *)&DAT_000aafb0;
    for (iVar8 = 7; iVar8 != 0; iVar8 = iVar8 + -1) {
      *puVar16 = *(undefined4 *)puVar12;
      puVar12 = puVar12 + (uint)bVar18 * -4 + 2;
      puVar16 = puVar16 + (uint)bVar18 * -2 + 1;
    }
    FUN_0007c794();
    return;
  case 0x22:
    FUN_0007e4d6();
    FUN_0007deaa();
                    /* WARNING: Could not recover jumptable at 0x0007cd50. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x24:
    FUN_0007e51c();
    FUN_0007deaa();
                    /* WARNING: Could not recover jumptable at 0x0007cef0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x25:
    puVar16 = (undefined4 *)&DAT_000aafb0;
    for (iVar8 = 7; iVar8 != 0; iVar8 = iVar8 + -1) {
      *puVar16 = *(undefined4 *)puVar12;
      puVar12 = puVar12 + (uint)bVar18 * -4 + 2;
      puVar16 = puVar16 + (uint)bVar18 * -2 + 1;
    }
    uVar11 = ((_DAT_000aafb4 & 0x3800) >> 0xb) * 10;
    iVar8 = 8;
    do {
      puVar15 = puVar12 + (uint)bVar18 * -4 + 2;
      *(undefined4 *)((int)&DAT_000aafca + uVar11 + 2) = *(undefined4 *)puVar12;
      *(undefined4 *)(&DAT_000aafd0 + (uint)bVar18 * -8 + uVar11) = *(undefined4 *)puVar15;
      puVar12 = puVar15 + (uint)bVar18 * -4 + 2 + (uint)bVar18 * -2 + 1;
      *(ushort *)((int)(&DAT_000aafd0 + (uint)bVar18 * -8 + uVar11) + ((uint)bVar18 * -2 + 1) * 4) =
           puVar15[(uint)bVar18 * -4 + 2];
      uVar11 = (uint)*(ushort *)(&DAT_0007c392 + uVar11);
      iVar8 = iVar8 + -1;
    } while (iVar8 != 0);
    FUN_0007c794();
    return;
  case 0x26:
    FUN_0007e4d6();
    FUN_0007deaa();
                    /* WARNING: Could not recover jumptable at 0x0007d145. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x27:
    FUN_0007c794();
    return;
  case 0x28:
    FUN_0007e6e2();
    FUN_0007deaa();
                    /* WARNING: Could not recover jumptable at 0x0007cba1. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x29:
    DAT_000aafb0 = *puVar12;
    if ((DAT_000aafb0 & 0x300) == 0x300) {
      DAT_000ab026 = FUN_0007c794;
    }
    else if ((DAT_000aafb0 & 0x300) == 0x200) {
      DAT_000ab026 = (code *)&LAB_0007c904;
    }
    else {
      DAT_000ab026 = (code *)&LAB_0007c8c8;
    }
    FUN_0007c794();
    return;
  case 0x2a:
    FUN_0007e4d6();
    FUN_0007deaa();
                    /* WARNING: Could not recover jumptable at 0x0007cd6f. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x2b:
    uVar11 = (uint)*(ushort *)(&DAT_0007c394 + uVar14);
    _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
    _DAT_000aafb4 = CONCAT22(DAT_000aafb4_2,DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + uVar11));
    DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar11);
    *(undefined4 *)((int)&DAT_000aafca + uVar11 + 2) = *(undefined4 *)puVar12;
    *(undefined4 *)(&DAT_000aafd0 + uVar11) = *(undefined4 *)(puVar12 + 2);
    *(ushort *)(&DAT_000aafd4 + uVar11) = puVar12[4];
    FUN_0007c794();
    return;
  case 0x2c:
    FUN_0007e51c();
    FUN_0007deaa();
                    /* WARNING: Could not recover jumptable at 0x0007cf13. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x2e:
    FUN_0007e4d6();
    FUN_0007deaa();
                    /* WARNING: Could not recover jumptable at 0x0007d166. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x2f:
    _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
    _DAT_000aafb4 =
         CONCAT22(DAT_000aafb4_2,
                  DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c394 + uVar14)));
    DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + *(ushort *)(&DAT_0007c394 + uVar14));
    FUN_0007e4e7();
    FUN_0007c794();
    return;
  case 0x30:
    FUN_0007e6e2();
    FUN_0007e117();
                    /* WARNING: Could not recover jumptable at 0x0007cbc0. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x31:
    puVar16 = (undefined4 *)&DAT_000aafb0;
    for (iVar8 = 7; iVar8 != 0; iVar8 = iVar8 + -1) {
      *(undefined4 *)puVar12 = *puVar16;
      puVar16 = puVar16 + (uint)bVar18 * -2 + 1;
      puVar12 = puVar12 + (uint)bVar18 * -4 + 2;
    }
    FUN_0007c794();
    return;
  case 0x32:
    FUN_0007e4d6();
    FUN_0007e117();
                    /* WARNING: Could not recover jumptable at 0x0007cd8e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x34:
    FUN_0007e51c();
    FUN_0007e117();
                    /* WARNING: Could not recover jumptable at 0x0007cf36. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x35:
    puVar16 = (undefined4 *)&DAT_000aafb0;
    for (iVar8 = 7; iVar8 != 0; iVar8 = iVar8 + -1) {
      *(undefined4 *)puVar12 = *puVar16;
      puVar16 = puVar16 + (uint)bVar18 * -2 + 1;
      puVar12 = puVar12 + (uint)bVar18 * -4 + 2;
    }
    iVar8 = 8;
    do {
      puVar15 = puVar12 + (uint)bVar18 * -4 + 2;
      *(undefined4 *)puVar12 = *(undefined4 *)((int)&DAT_000aafca + uVar14 + 2);
      *(undefined4 *)puVar15 = *(undefined4 *)(&DAT_000aafd0 + (uint)bVar18 * -8 + uVar14);
      puVar12 = puVar15 + (uint)bVar18 * -4 + 2 + (uint)bVar18 * -2 + 1;
      puVar15[(uint)bVar18 * -4 + 2] =
           *(ushort *)
            ((int)(&DAT_000aafd0 + (uint)bVar18 * -8 + uVar14) + ((uint)bVar18 * -2 + 1) * 4);
      uVar14 = (uint)*(ushort *)(&DAT_0007c392 + uVar14);
      iVar8 = iVar8 + -1;
    } while (iVar8 != 0);
    switchD_0007d953::caseD_3();
    return;
  case 0x36:
    FUN_0007e4d6();
    FUN_0007e117();
                    /* WARNING: Could not recover jumptable at 0x0007d187. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x37:
    break;
  case 0x38:
    FUN_0007e6e2();
    FUN_0007e117();
                    /* WARNING: Could not recover jumptable at 0x0007cbdf. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x39:
    *puVar12 = DAT_000aafb0;
    FUN_0007c794();
    return;
  case 0x3a:
    FUN_0007e4d6();
    FUN_0007e117();
                    /* WARNING: Could not recover jumptable at 0x0007cdad. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x3b:
    *(undefined4 *)puVar12 = *(undefined4 *)((int)&DAT_000aafca + uVar14 + 2);
    *(undefined4 *)(puVar12 + 2) = *(undefined4 *)(&DAT_000aafd0 + uVar14);
    puVar12[4] = *(ushort *)(&DAT_000aafd4 + uVar14);
    break;
  case 0x3c:
    FUN_0007e51c();
    FUN_0007e117();
                    /* WARNING: Could not recover jumptable at 0x0007cf59. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x3d:
    *puVar12 = DAT_000aafb4;
    FUN_0007c794();
    return;
  case 0x3e:
    FUN_0007e4d6();
    FUN_0007e117();
                    /* WARNING: Could not recover jumptable at 0x0007d1a8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x3f:
    FUN_0007e7a7();
    uVar6 = FUN_0007e46c();
    *(undefined4 *)puVar12 = uVar6;
    break;
  case 0x40:
switchD_0007c812_caseD_40:
    FUN_0007deb8();
                    /* WARNING: Could not recover jumptable at 0x0007d30b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x41:
switchD_0007c812_caseD_41:
    uVar11 = (uint)*(ushort *)(&DAT_0007c394 + uVar14);
    _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
    _DAT_000aafb4 = CONCAT22(DAT_000aafb4_2,DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + uVar11));
    DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar11);
    *(undefined4 *)((int)&DAT_000aafca + uVar11 + 2) = *(undefined4 *)(puVar12 + 0x557e6);
    *(undefined4 *)(&DAT_000aafd0 + uVar11) = *(undefined4 *)(puVar12 + 0x557e8);
    *(ushort *)(&DAT_000aafd4 + uVar11) = puVar12[0x557ea];
    FUN_0007c794();
    return;
  default:
    goto switchD_0007c812_caseD_42;
  case 0x44:
switchD_0007c812_caseD_44:
    FUN_0007deb8();
                    /* WARNING: Could not recover jumptable at 0x0007d9be. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x45:
  case 0x47:
switchD_0007c812_caseD_45:
    DAT_000aafb8 = DAT_000aafb8 & puVar12[0x3e1f1] | puVar12[0x3e1f5];
    FUN_0007c794();
    return;
  case 0x46:
switchD_0007c812_caseD_46:
    FUN_0007deb8();
    DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar14) |
                   *(ushort *)(&DAT_0007c3ea + uVar14);
    _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
    _DAT_000aafb4 =
         CONCAT22(DAT_000aafb4_2,
                  DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c392 + uVar14)));
                    /* WARNING: Could not recover jumptable at 0x0007db0d. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x48:
switchD_0007c812_caseD_48:
    FUN_0007e2ec();
                    /* WARNING: Could not recover jumptable at 0x0007d320. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x49:
  case 0x4d:
  case 0x4f:
switchD_0007c812_caseD_49:
    puVar16 = (undefined4 *)((int)&DAT_000aafca + uVar14 + 2);
    LOCK();
    uVar6 = *puVar16;
    *puVar16 = *(undefined4 *)(puVar12 + 0x557e6);
    UNLOCK();
    *(undefined4 *)(puVar12 + 0x557e6) = uVar6;
    LOCK();
    uVar6 = *(undefined4 *)(&DAT_000aafd0 + uVar14);
    *(undefined4 *)(&DAT_000aafd0 + uVar14) = *(undefined4 *)(puVar12 + 0x557e8);
    UNLOCK();
    *(undefined4 *)(puVar12 + 0x557e8) = uVar6;
    LOCK();
    uVar7 = *(ushort *)(&DAT_000aafd4 + uVar14);
    *(ushort *)(&DAT_000aafd4 + uVar14) = puVar12[0x557ea];
    UNLOCK();
    puVar12[0x557ea] = uVar7;
    FUN_0007c794();
    return;
  case 0x4c:
switchD_0007c812_caseD_4c:
    FUN_0007e2ec();
                    /* WARNING: Could not recover jumptable at 0x0007d9d3. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x4e:
switchD_0007c812_caseD_4e:
    FUN_0007e2ec();
    DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar14) |
                   *(ushort *)(&DAT_0007c3ea + uVar14);
    _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
    _DAT_000aafb4 =
         CONCAT22(DAT_000aafb4_2,
                  DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c392 + uVar14)));
                    /* WARNING: Could not recover jumptable at 0x0007db58. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x50:
  case 0x54:
switchD_0007c812_caseD_50:
    FUN_0007e07e();
    FUN_0007c988();
    return;
  case 0x51:
    goto switchD_0007c812_caseD_51;
  case 0x55:
  case 0x57:
switchD_0007c812_caseD_55:
    *(undefined4 *)(puVar12 + 0x557e6) = *(undefined4 *)((int)&DAT_000aafca + uVar14 + 2);
    *(undefined4 *)(puVar12 + 0x557e8) = *(undefined4 *)(&DAT_000aafd0 + uVar14);
    puVar12[0x557ea] = *(ushort *)(&DAT_000aafd4 + uVar14);
    DAT_000aafb8 = DAT_000aafb8 & puVar12[0x3e1f1];
    FUN_0007c794();
    return;
  case 0x58:
  case 0x5c:
switchD_0007c812_caseD_58:
    FUN_0007e07e();
    FUN_0007c936();
    return;
  case 0x59:
  case 0x5d:
  case 0x5f:
switchD_0007c812_caseD_59:
    *(undefined4 *)(puVar12 + 0x557e6) = *(undefined4 *)((int)&DAT_000aafca + uVar14 + 2);
    *(undefined4 *)(puVar12 + 0x557e8) = *(undefined4 *)(&DAT_000aafd0 + uVar14);
    puVar12[0x557ea] = *(ushort *)(&DAT_000aafd4 + uVar14);
    DAT_000aafb8 = DAT_000aafb8 & puVar12[0x3e1f1];
    break;
  case 0x5e:
switchD_0007c812_caseD_5e:
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
    switch(uVar13) {
    case 1:
      DAT_000ab01c = *(undefined4 *)((int)&DAT_000aafca + uVar14 + 2);
      DAT_000ab020 = *(undefined4 *)(&DAT_000aafd0 + uVar14);
      DAT_000ab024 = *(undefined2 *)(&DAT_000aafd4 + uVar14);
      DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar14) |
                     *(ushort *)(&DAT_0007c3ea + uVar14);
      _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
      _DAT_000aafb4 =
           CONCAT22(DAT_000aafb4_2,
                    DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c392 + uVar14)))
      ;
      FUN_0007e07e();
      FUN_0007c936();
      return;
    }
    goto switchD_0007c812_caseD_42;
  case 0x60:
switchD_0007c812_caseD_60:
    FUN_0007deaa();
                    /* WARNING: Could not recover jumptable at 0x0007d359. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x61:
switchD_0007c812_caseD_61:
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
    switch(uVar13) {
    case 0:
      (&DAT_000aafd5)[uVar14] = (&DAT_000aafd5)[uVar14] ^ 0x80;
      break;
    case 1:
      (&DAT_000aafd5)[uVar14] = (&DAT_000aafd5)[uVar14] & 0x7f;
      break;
    default:
      goto switchD_0007c812_caseD_42;
    case 4:
      DAT_000ab01c = DAT_0007c388;
      DAT_000ab020 = DAT_0007c38c;
      DAT_000ab024 = DAT_0007c390;
      FUN_0007e07e();
      FUN_0007c988();
      return;
    case 5:
      FUN_0007ec2e();
    }
    goto code_r0x0007c794;
  case 99:
switchD_0007c812_caseD_63:
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
    switch(uVar13) {
    default:
      goto switchD_0007c812_caseD_42;
    case 2:
      _DAT_000aafb4 = _DAT_000aafb4 & 0xffff7f00;
      break;
    case 3:
      DAT_000aafb0 = 0x33f;
      _DAT_000aafb4 = _DAT_000aafb4 & 0xffff0000;
      DAT_000aafb8 = 0xffff;
      DAT_000ab026 = FUN_0007c794;
      FUN_0007c794();
      return;
    }
    goto code_r0x0007c794;
  case 100:
switchD_0007c812_caseD_64:
    FUN_0007deaa();
                    /* WARNING: Could not recover jumptable at 0x0007d9e8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x65:
switchD_0007c812_caseD_65:
    FUN_0007e07e();
    FUN_0007c988();
    return;
  case 0x66:
switchD_0007c812_caseD_66:
    FUN_0007deaa();
    DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar14) |
                   *(ushort *)(&DAT_0007c3ea + uVar14);
    _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
    _DAT_000aafb4 =
         CONCAT22(DAT_000aafb4_2,
                  DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c392 + uVar14)));
                    /* WARNING: Could not recover jumptable at 0x0007dc2c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x67:
switchD_0007c812_caseD_67:
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
    switch(uVar13) {
    case 0:
      break;
    default:
      goto switchD_0007c812_caseD_42;
    }
    goto code_r0x0007c794;
  case 0x68:
switchD_0007c812_caseD_68:
    FUN_0007deaa();
                    /* WARNING: Could not recover jumptable at 0x0007d36e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x69:
switchD_0007c812_caseD_69:
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
    switch(uVar13) {
    case 0:
      uVar11 = (uint)*(ushort *)(&DAT_0007c394 + uVar14);
      _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
      _DAT_000aafb4 = CONCAT22(DAT_000aafb4_2,DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + uVar11));
      DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar11);
      *(undefined4 *)((int)&DAT_000aafca + uVar11 + 2) = 0;
      *(undefined4 *)(&DAT_000aafd0 + uVar11) = 0x80000000;
      *(undefined2 *)(&DAT_000aafd4 + uVar11) = 0x3fff;
      break;
    case 1:
      uVar11 = (uint)*(ushort *)(&DAT_0007c394 + uVar14);
      _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
      _DAT_000aafb4 = CONCAT22(DAT_000aafb4_2,DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + uVar11));
      DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar11);
      *(undefined4 *)((int)&DAT_000aafca + uVar11 + 2) = 0xcd1b8afe;
      *(undefined4 *)(&DAT_000aafd0 + uVar11) = 0xd49a784b;
      *(undefined2 *)(&DAT_000aafd4 + uVar11) = 0x4000;
      break;
    case 2:
      uVar11 = (uint)*(ushort *)(&DAT_0007c394 + uVar14);
      _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
      _DAT_000aafb4 = CONCAT22(DAT_000aafb4_2,DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + uVar11));
      DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar11);
      *(undefined4 *)((int)&DAT_000aafca + uVar11 + 2) = 0x5c17f0bc;
      *(undefined4 *)(&DAT_000aafd0 + uVar11) = 0xb8aa3b29;
      *(undefined2 *)(&DAT_000aafd4 + uVar11) = 0x3fff;
      break;
    case 3:
      uVar11 = (uint)*(ushort *)(&DAT_0007c394 + uVar14);
      _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
      _DAT_000aafb4 = CONCAT22(DAT_000aafb4_2,DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + uVar11));
      DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar11);
      *(undefined4 *)((int)&DAT_000aafca + uVar11 + 2) = 0x2168c235;
      *(undefined4 *)(&DAT_000aafd0 + uVar11) = 0xc900daa2;
      *(undefined2 *)(&DAT_000aafd4 + uVar11) = 0x4000;
      break;
    case 4:
      uVar11 = (uint)*(ushort *)(&DAT_0007c394 + uVar14);
      _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
      _DAT_000aafb4 = CONCAT22(DAT_000aafb4_2,DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + uVar11));
      DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar11);
      *(undefined4 *)((int)&DAT_000aafca + uVar11 + 2) = 0xfbcff799;
      *(undefined4 *)(&DAT_000aafd0 + uVar11) = 0x9a209a84;
      *(undefined2 *)(&DAT_000aafd4 + uVar11) = 0x3ffd;
      break;
    case 5:
      uVar11 = (uint)*(ushort *)(&DAT_0007c394 + uVar14);
      _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
      _DAT_000aafb4 = CONCAT22(DAT_000aafb4_2,DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + uVar11));
      DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar11);
      *(undefined4 *)((int)&DAT_000aafca + uVar11 + 2) = 0xd1cf79ac;
      *(undefined4 *)(&DAT_000aafd0 + uVar11) = 0xb17217f7;
      *(undefined2 *)(&DAT_000aafd4 + uVar11) = 0x3ffe;
      break;
    case 6:
      uVar11 = (uint)*(ushort *)(&DAT_0007c394 + uVar14);
      _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
      _DAT_000aafb4 = CONCAT22(DAT_000aafb4_2,DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + uVar11));
      DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar11);
      *(undefined4 *)((int)&DAT_000aafca + uVar11 + 2) = 0;
      *(undefined4 *)(&DAT_000aafd0 + uVar11) = 0;
      *(undefined2 *)(&DAT_000aafd4 + uVar11) = 0;
      DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar11) |
                     *(ushort *)(&DAT_0007c3e4 + uVar11);
      break;
    case 7:
      goto switchD_0007c812_caseD_42;
    }
    goto code_r0x0007c794;
  case 0x6a:
switchD_0007c812_caseD_6a:
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
    switch(uVar13) {
    case 1:
      DAT_000ab01c = *(undefined4 *)((int)&DAT_000aafca + uVar14 + 2);
      DAT_000ab020 = *(undefined4 *)(&DAT_000aafd0 + uVar14);
      DAT_000ab024 = *(undefined2 *)(&DAT_000aafd4 + uVar14);
      DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar14) |
                     *(ushort *)(&DAT_0007c3ea + uVar14);
      _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
      _DAT_000aafb4 =
           CONCAT22(DAT_000aafb4_2,
                    DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c392 + uVar14)))
      ;
      FUN_0007e07e();
      FUN_0007c936();
      return;
    }
    goto switchD_0007c812_caseD_42;
  case 0x6c:
switchD_0007c812_caseD_6c:
    FUN_0007deaa();
                    /* WARNING: Could not recover jumptable at 0x0007d9fd. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x6d:
switchD_0007c812_caseD_6d:
    FUN_0007e07e();
    FUN_0007c936();
    return;
  case 0x6e:
switchD_0007c812_caseD_6e:
    FUN_0007deaa();
    DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar14) |
                   *(ushort *)(&DAT_0007c3ea + uVar14);
    _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
    _DAT_000aafb4 =
         CONCAT22(DAT_000aafb4_2,
                  DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c392 + uVar14)));
                    /* WARNING: Could not recover jumptable at 0x0007dc77. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x70:
switchD_0007c812_caseD_70:
    FUN_0007e117();
                    /* WARNING: Could not recover jumptable at 0x0007d383. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x71:
switchD_0007c812_caseD_71:
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
    switch(uVar13) {
    case 0:
      FUN_0007f254();
                    /* WARNING: Could not recover jumptable at 0x0007d6d9. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*DAT_000ab026)();
      return;
    case 1:
      DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar14) |
                     *(ushort *)(&DAT_0007c3ea + uVar14);
      _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
      _DAT_000aafb4 =
           CONCAT22(DAT_000aafb4_2,
                    DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c392 + uVar14)))
      ;
      FUN_0007ed35();
                    /* WARNING: Could not recover jumptable at 0x0007d720. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*DAT_000ab026)();
      return;
    case 2:
      FUN_0007f21e();
      uVar11 = (uint)*(ushort *)(&DAT_0007c394 + uVar14);
      _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
      _DAT_000aafb4 = CONCAT22(DAT_000aafb4_2,DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + uVar11));
      DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar11);
      *(undefined4 *)((int)&DAT_000aafca + uVar11 + 2) = 0;
      *(undefined4 *)(&DAT_000aafd0 + uVar11) = 0x80000000;
      *(undefined2 *)(&DAT_000aafd4 + uVar11) = 0x3fff;
      break;
    case 3:
      DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar14) |
                     *(ushort *)(&DAT_0007c3ea + uVar14);
      _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
      _DAT_000aafb4 =
           CONCAT22(DAT_000aafb4_2,
                    DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c392 + uVar14)))
      ;
      FUN_0007e934();
                    /* WARNING: Could not recover jumptable at 0x0007dd7e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*DAT_000ab026)();
      return;
    case 4:
      uVar11 = (uint)*(ushort *)(&DAT_0007c394 + uVar14);
      _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
      _DAT_000aafb4 = CONCAT22(DAT_000aafb4_2,DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + uVar11));
      DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar11);
      *(undefined4 *)((int)&DAT_000aafca + uVar11 + 2) =
           *(undefined4 *)((int)&DAT_000aafca + uVar14 + 2);
      *(undefined4 *)(&DAT_000aafd0 + uVar11) = *(undefined4 *)(&DAT_000aafd0 + uVar14);
      *(ushort *)(&DAT_000aafd4 + uVar11) = *(ushort *)(&DAT_000aafd4 + uVar14) & 0x8000 | 0x3fff;
      FUN_0007e467();
      break;
    case 5:
      uVar11 = FUN_0007eb4d();
      _DAT_000aafb4 =
           CONCAT22(DAT_000aafb4_2,
                    CONCAT11((byte)((DAT_000aafb4 & 0xb8ff) >> 8) | (&DAT_0007c436)[uVar11 & 7],
                             (char)(DAT_000aafb4 & 0xb8ff)));
                    /* WARNING: Could not recover jumptable at 0x0007de0d. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*DAT_000ab026)();
      return;
    case 6:
      _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
      _DAT_000aafb4 =
           CONCAT22(DAT_000aafb4_2,
                    DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c394 + uVar14)))
      ;
      break;
    case 7:
      _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
      _DAT_000aafb4 =
           CONCAT22(DAT_000aafb4_2,
                    DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c392 + uVar14)))
      ;
    }
    goto code_r0x0007c794;
  case 0x74:
switchD_0007c812_caseD_74:
    FUN_0007e117();
                    /* WARNING: Could not recover jumptable at 0x0007da12. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x76:
switchD_0007c812_caseD_76:
    FUN_0007e117();
    DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar14) |
                   *(ushort *)(&DAT_0007c3ea + uVar14);
    _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
    _DAT_000aafb4 =
         CONCAT22(DAT_000aafb4_2,
                  DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c392 + uVar14)));
                    /* WARNING: Could not recover jumptable at 0x0007dcc2. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x78:
switchD_0007c812_caseD_78:
    FUN_0007e117();
                    /* WARNING: Could not recover jumptable at 0x0007d398. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x79:
    goto switchD_0007c812_caseD_79;
  case 0x7c:
switchD_0007c812_caseD_7c:
    FUN_0007e117();
                    /* WARNING: Could not recover jumptable at 0x0007da27. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 0x7e:
switchD_0007c812_caseD_7e:
    FUN_0007e117();
    DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar14) |
                   *(ushort *)(&DAT_0007c3ea + uVar14);
    _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
    _DAT_000aafb4 =
         CONCAT22(DAT_000aafb4_2,
                  DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c392 + uVar14)));
                    /* WARNING: Could not recover jumptable at 0x0007dd0d. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  }
  DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar14) |
                 *(ushort *)(&DAT_0007c3ea + uVar14);
  _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
  _DAT_000aafb4 =
       CONCAT22(DAT_000aafb4_2,
                DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c392 + uVar14)));
  FUN_0007c794();
  return;
switchD_0007c812_caseD_79:
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
  switch(uVar13) {
  case 0:
    uVar11 = FUN_0007eb4d();
    _DAT_000aafb4 =
         CONCAT22(DAT_000aafb4_2,
                  CONCAT11((byte)((DAT_000aafb4 & 0xb8ff) >> 8) | (&DAT_0007c436)[uVar11 & 7],
                           (char)(DAT_000aafb4 & 0xb8ff)));
                    /* WARNING: Could not recover jumptable at 0x0007d800. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 1:
    DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar14) |
                   *(ushort *)(&DAT_0007c3ea + uVar14);
    _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
    _DAT_000aafb4 =
         CONCAT22(DAT_000aafb4_2,
                  DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c392 + uVar14)));
    FUN_0007ed1c();
                    /* WARNING: Could not recover jumptable at 0x0007d847. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 2:
    FUN_0007f428();
                    /* WARNING: Could not recover jumptable at 0x0007d854. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 3:
    uVar6 = *(undefined4 *)((int)&DAT_000aafca + uVar14 + 2);
    uVar1 = *(undefined4 *)(&DAT_000aafd0 + uVar14);
    uVar2 = *(undefined4 *)(&DAT_000aafd2 + uVar14);
    FUN_0007f207();
    uVar11 = (uint)*(ushort *)(&DAT_0007c394 + uVar14);
    _DAT_000aafb4 = _DAT_000aafb4 & 0xffffc7ff;
    _DAT_000aafb4 = CONCAT22(DAT_000aafb4_2,DAT_000aafb4 | *(ushort *)(&DAT_0007c396 + uVar11));
    DAT_000aafb8 = DAT_000aafb8 & *(ushort *)(&DAT_0007c3e2 + uVar11);
    *(undefined4 *)(&DAT_000aafd2 + uVar11) = uVar2;
    *(undefined4 *)(&DAT_000aafd0 + uVar11) = uVar1;
    *(undefined4 *)((int)&DAT_000aafca + uVar11 + 2) = uVar6;
    FUN_0007f211();
    break;
  case 4:
    FUN_0007e7a7();
    break;
  case 5:
    iVar8 = FUN_0007e467();
    sVar4 = (short)iVar8;
    if (sVar4 == iVar8) {
      uVar7 = *(ushort *)(&DAT_000aafd4 + uVar14) & 0x8000;
      uVar9 = (*(ushort *)(&DAT_000aafd4 + uVar14) & 0x7fff) + sVar4;
      if ((short)uVar9 < 0) {
        if (sVar4 < 0) {
          uVar9 = 0;
          uVar7 = 0;
        }
        else {
          uVar9 = 0x7fff;
        }
      }
    }
    else if (extraout_CX < 0) {
      uVar9 = 0;
      uVar7 = 0;
    }
    else {
      uVar9 = 0x7fff;
      uVar7 = *(ushort *)(&DAT_000aafd4 + uVar14) & 0x8000;
    }
    *(ushort *)(&DAT_000aafd4 + uVar14) = uVar9 | uVar7;
    break;
  case 6:
    FUN_0007f207();
                    /* WARNING: Could not recover jumptable at 0x0007d8b9. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  case 7:
    FUN_0007f211();
                    /* WARNING: Could not recover jumptable at 0x0007d8c6. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*DAT_000ab026)();
    return;
  }
  goto code_r0x0007c794;
switchD_0007c812_caseD_51:
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
  switch(uVar13) {
  case 0:
    break;
  default:
switchD_0007c812_caseD_42:
    FUN_0007c794();
    return;
  }
  goto code_r0x0007c794;
}

