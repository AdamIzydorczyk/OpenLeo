
int __regparm3 FUN_0005f3d0(undefined4 *param_1)

{
  int *piVar1;
  code *pcVar2;
  int *piVar3;
  bool bVar4;
  int iVar5;
  undefined4 uVar6;
  int iVar7;
  undefined4 *puVar8;
  undefined4 *extraout_ECX;
  undefined4 *extraout_ECX_00;
  undefined4 *extraout_ECX_01;
  undefined4 *extraout_ECX_02;
  undefined4 *extraout_ECX_03;
  undefined4 *extraout_ECX_04;
  undefined4 *extraout_ECX_05;
  undefined4 *extraout_ECX_06;
  undefined4 *extraout_ECX_07;
  undefined4 *extraout_ECX_08;
  undefined4 *extraout_ECX_09;
  undefined1 *puVar9;
  int aiStack_16c [5];
  undefined1 local_158;
  int local_157 [2];
  undefined **local_14f;
  undefined4 local_14b;
  undefined4 local_147;
  int local_13f [3];
  undefined **local_130 [43];
  undefined **local_84 [4];
  undefined **local_74 [14];
  int local_3c [11];
  
  *param_1 = 0;
  param_1[1] = 0;
  param_1[4] = &PTR_LAB_00098278;
  (**(code **)(param_1[4] + 4))();
  param_1[5] = 0;
  param_1[6] = 0;
  param_1[7] = 0;
  param_1[8] = 0;
  param_1[9] = 0;
  param_1[10] = 0;
  FUN_0004c080();
  FUN_0004bff0();
  FUN_0004bf60();
  iVar5 = FUN_0004bed0();
  *(undefined4 *)(iVar5 + 0x16) = 0;
  *(undefined4 *)(iVar5 + 0x1a) = 0;
  *(undefined4 *)(iVar5 + 0x1e) = 0;
  *(undefined4 *)(iVar5 + 0x22) = 0;
  *(undefined4 *)(iVar5 + 0x26) = 0;
  *(undefined4 *)(iVar5 + 0x2a) = 0;
  *(undefined ***)(iVar5 + 0x2e) = &PTR_FUN_000a2db4;
  FUN_00057a70();
  FUN_00057a70();
  local_157[0] = 0;
  local_157[1] = 0;
  local_14b = 0;
  local_147 = 0;
  local_14f = &PTR_FUN_000961fc;
  FUN_000497b0();
  FUN_00057a00();
  FUN_00057a00();
  local_84[1] = &PTR_FUN_000961fc;
  local_130[0x24] = &PTR_FUN_00096200;
  FUN_00057990();
  local_130[0x24] = &PTR_FUN_000961fc;
  local_74[1] = &PTR_FUN_000961fc;
  uVar6 = thunk_FUN_0005c320();
  puVar9 = &local_158;
  *(undefined4 *)(iVar5 + -0x5a) = uVar6;
  local_3c[6] = iVar5;
  local_3c[7] = iVar5 + -0x2c;
  piVar1 = (int *)(iVar5 + -0x42);
  local_3c[10] = iVar5 + -0x16;
  local_3c[8] = iVar5 + -0x4a;
  local_3c[5] = iVar5 + -0x4e;
  local_3c[9] = iVar5 + 0x1e;
  do {
    if (*(int **)(puVar9 + 1) == (int *)0x0) {
LAB_0005f5d5:
      bVar4 = true;
    }
    else {
      pcVar2 = *(code **)(**(int **)(puVar9 + 1) + 0x18);
      *(undefined4 *)(puVar9 + -4) = 0x5f5c8;
      iVar7 = (*pcVar2)();
      bVar4 = false;
      if (iVar7 == -1) goto LAB_0005f5d5;
    }
    if (bVar4) {
      if (*(int *)(iVar5 + -0x52) != 0) {
        *(undefined4 *)(puVar9 + -4) = 0x60288;
        FUN_0001cab0();
      }
      if ((*(char **)(puVar9 + 0xb4) == (char *)0x0) || (**(char **)(puVar9 + 0xb4) == '\0')) {
        bVar4 = true;
      }
      else {
        bVar4 = false;
      }
      if (!bVar4) {
        *(undefined4 *)(puVar9 + -4) = 0x602c0;
        FUN_0005f190();
      }
      *(undefined4 *)(puVar9 + -4) = 0x602d1;
      FUN_00057a00();
      *(undefined4 *)(puVar9 + -4) = 0x602e2;
      FUN_00057a00();
      *(undefined4 *)(puVar9 + -4) = 0x602f3;
      FUN_00057a00();
      *(undefined4 *)(puVar9 + -4) = 0x602fa;
      FUN_00049a10();
      *(undefined4 *)(puVar9 + -4) = 0x60305;
      FUN_00057a00();
      return iVar5 + -0x6e;
    }
    *(undefined4 *)(puVar9 + -4) = 0x5f5f7;
    FUN_00057d30();
    if (**(char **)(puVar9 + 0xd4) != ';') {
      *(undefined4 *)(puVar9 + -4) = 0x5f61b;
      iVar7 = FUN_00076d20();
      if (iVar7 == 0) {
        *(undefined1 **)(puVar9 + -4) = puVar9 + 0xc0;
        *(undefined1 **)(puVar9 + -8) = puVar9 + 0xbc;
        *(char **)(puVar9 + -0xc) = s____d____d_00092fa4;
        *(undefined1 **)(puVar9 + -0x10) = puVar9;
        *(undefined4 *)(puVar9 + 0xbc) = 0;
        *(undefined4 *)(puVar9 + 0xc0) = 0;
        *(undefined4 *)(puVar9 + -0x14) = 0x5f65a;
        FUN_00049920();
        *(undefined4 *)(puVar9 + -4) = 0x5f667;
        puVar8 = (undefined4 *)FUN_0005c320();
        if (puVar8 != (undefined4 *)0x0) {
          *puVar8 = *(undefined4 *)(puVar9 + 0xbc);
          puVar8[1] = *(undefined4 *)(puVar9 + 0xc0);
        }
        piVar3 = *(int **)(puVar9 + 0x134);
        *(undefined4 **)(puVar9 + 0xec) = puVar8;
        puVar8 = (undefined4 *)(puVar9 + 0xec);
        if (piVar3[2] == piVar3[3]) {
          *(undefined4 *)(puVar9 + -4) = 0x5f6a7;
          FUN_0004c0a0();
          puVar8 = extraout_ECX;
        }
        iVar7 = piVar3[2];
        piVar3[2] = iVar7 + 1;
        *(undefined4 *)(iVar7 * 4 + *piVar3) = *puVar8;
      }
      else {
        *(undefined4 *)(puVar9 + -4) = 0x5f6d3;
        iVar7 = FUN_00076d20();
        if (iVar7 == 0) {
          *(undefined1 **)(puVar9 + -4) = puVar9 + 0x2c;
          *(undefined1 **)(puVar9 + -8) = puVar9 + 0x28;
          *(undefined4 *)(puVar9 + 0x28) = 0;
          *(char **)(puVar9 + -0xc) = s____d____d___00092fb7;
          *(undefined4 *)(puVar9 + 0x2c) = 0;
          *(undefined1 **)(puVar9 + -0x10) = puVar9;
          *(undefined ***)(puVar9 + 0x30) = &PTR_LAB_00098278;
          *(undefined4 *)(puVar9 + -0x14) = 0x5f718;
          FUN_00049920();
          *(undefined4 *)(puVar9 + -4) = 0x5f729;
          FUN_00057d30();
          *(undefined4 *)(puVar9 + -4) = 0x5f733;
          iVar7 = FUN_0005c320();
          uVar6 = 0;
          if (iVar7 != 0) {
            *(undefined4 *)(puVar9 + -4) = 0x5f749;
            uVar6 = FUN_0005ec20();
          }
          piVar3 = *(int **)(puVar9 + 0x138);
          *(undefined4 *)(puVar9 + 0xf0) = uVar6;
          puVar8 = (undefined4 *)(puVar9 + 0xf0);
          if (piVar3[2] == piVar3[3]) {
            *(undefined4 *)(puVar9 + -4) = 0x5f770;
            FUN_0004c3c0();
            puVar8 = extraout_ECX_00;
          }
          iVar7 = piVar3[2];
          piVar3[2] = iVar7 + 1;
          *(undefined4 *)(iVar7 * 4 + *piVar3) = *puVar8;
        }
        else {
          *(undefined4 *)(puVar9 + -4) = 0x5f79c;
          iVar7 = FUN_00076d20();
          if (iVar7 == 0) {
            *(undefined1 **)(puVar9 + -4) = puVar9 + 0x74;
            *(undefined1 **)(puVar9 + -8) = puVar9 + 0x70;
            *(undefined4 *)(puVar9 + 0x70) = 0;
            *(char **)(puVar9 + -0xc) = s____d____d___00092fd4;
            *(undefined4 *)(puVar9 + 0x74) = 0;
            *(undefined1 **)(puVar9 + -0x10) = puVar9;
            *(undefined ***)(puVar9 + 0x78) = &PTR_LAB_00098278;
            *(undefined4 *)(puVar9 + -0x14) = 0x5f7e7;
            FUN_00049920();
            *(undefined4 *)(puVar9 + -4) = 0x5f7f8;
            FUN_00057d30();
            *(undefined1 **)(puVar9 + -4) = puVar9 + 0xf4;
            *(char **)(puVar9 + -8) = s____d_00092fe9;
            *(undefined1 **)(puVar9 + -0xc) = puVar9;
            *(undefined4 *)(puVar9 + -0x10) = 0x5f80f;
            FUN_00049920();
            *(undefined4 *)(puVar9 + -4) = 0x5f81c;
            iVar7 = FUN_0005c320();
            uVar6 = 0;
            if (iVar7 != 0) {
              *(undefined4 *)(puVar9 + -4) = *(undefined4 *)(puVar9 + 0xf4);
              *(undefined4 *)(puVar9 + -8) = 0x5f83a;
              uVar6 = FUN_0005ed40();
              puVar9 = puVar9 + -4;
            }
            piVar3 = *(int **)(puVar9 + 0x144);
            *(undefined4 *)(puVar9 + 0xf8) = uVar6;
            puVar8 = (undefined4 *)(puVar9 + 0xf8);
            if (piVar3[2] == piVar3[3]) {
              *(undefined4 *)(puVar9 + -4) = 0x5f861;
              FUN_0004c230();
              puVar8 = extraout_ECX_01;
            }
            iVar7 = piVar3[2];
            piVar3[2] = iVar7 + 1;
            *(undefined4 *)(iVar7 * 4 + *piVar3) = *puVar8;
          }
          else {
            *(undefined4 *)(puVar9 + -4) = 0x5f88d;
            iVar7 = FUN_00076d20();
            if (iVar7 == 0) {
              *(undefined1 **)(puVar9 + -4) = puVar9 + 0x50;
              *(undefined1 **)(puVar9 + -8) = puVar9 + 0x4c;
              *(undefined4 *)(puVar9 + 0x4c) = 0;
              *(char **)(puVar9 + -0xc) = s____d____d___00092ff7;
              *(undefined4 *)(puVar9 + 0x50) = 0;
              *(undefined1 **)(puVar9 + -0x10) = puVar9;
              *(undefined ***)(puVar9 + 0x54) = &PTR_LAB_00098278;
              *(undefined4 *)(puVar9 + -0x14) = 0x5f8d2;
              FUN_00049920();
              *(undefined4 *)(puVar9 + -4) = 0x5f8e3;
              FUN_00057d30();
              *(undefined1 **)(puVar9 + -4) = puVar9 + 0xfc;
              *(char **)(puVar9 + -8) = s____d_0009300c;
              *(undefined1 **)(puVar9 + -0xc) = puVar9;
              *(undefined4 *)(puVar9 + -0x10) = 0x5f8fa;
              FUN_00049920();
              *(undefined4 *)(puVar9 + -4) = 0x5f907;
              iVar7 = FUN_0005c320();
              uVar6 = 0;
              if (iVar7 != 0) {
                *(undefined4 *)(puVar9 + -4) = *(undefined4 *)(puVar9 + 0xfc);
                *(undefined4 *)(puVar9 + -8) = 0x5f925;
                uVar6 = FUN_0005edf0();
                puVar9 = puVar9 + -4;
              }
              piVar3 = *(int **)(puVar9 + 0x144);
              *(undefined4 *)(puVar9 + 0x100) = uVar6;
              puVar8 = (undefined4 *)(puVar9 + 0x100);
              if (piVar3[2] == piVar3[3]) {
                *(undefined4 *)(puVar9 + -4) = 0x5f94c;
                FUN_0004c230();
                puVar8 = extraout_ECX_02;
              }
              iVar7 = piVar3[2];
              piVar3[2] = iVar7 + 1;
              *(undefined4 *)(iVar7 * 4 + *piVar3) = *puVar8;
            }
            else {
              *(undefined4 *)(puVar9 + -4) = 0x5f978;
              iVar7 = FUN_00076d20();
              if (iVar7 == 0) {
                *(undefined ***)(puVar9 + 0xa8) = &PTR_LAB_00098278;
                *(undefined1 **)(puVar9 + -4) = puVar9 + 0xa4;
                *(undefined1 **)(puVar9 + -8) = puVar9 + 0xa0;
                *(char **)(puVar9 + -0xc) = s____d____d___0009301a;
                *(undefined1 **)(puVar9 + -0x10) = puVar9;
                *(undefined4 *)(puVar9 + 0xa0) = 0;
                *(undefined4 *)(puVar9 + 0xa4) = 0;
                *(undefined4 *)(puVar9 + -0x14) = 0x5f9c9;
                FUN_00049920();
                *(undefined4 *)(puVar9 + -4) = 0x5f9da;
                FUN_00057d30();
                *(undefined1 **)(puVar9 + -4) = puVar9 + 0x104;
                *(char **)(puVar9 + -8) = s____d_0009302f;
                *(undefined1 **)(puVar9 + -0xc) = puVar9;
                *(undefined4 *)(puVar9 + -0x10) = 0x5f9f1;
                FUN_00049920();
                *(undefined4 *)(puVar9 + -4) = 0x5f9fe;
                iVar7 = FUN_0005c320();
                uVar6 = 0;
                if (iVar7 != 0) {
                  *(undefined4 *)(puVar9 + -4) = *(undefined4 *)(puVar9 + 0x104);
                  *(undefined4 *)(puVar9 + -8) = 0x5fa1f;
                  uVar6 = FUN_0005eeb0();
                  puVar9 = puVar9 + -4;
                }
                *(undefined4 *)(puVar9 + 0x108) = uVar6;
                puVar8 = (undefined4 *)(puVar9 + 0x108);
                if (*(int *)(iVar5 + -0x3a) == *(int *)(iVar5 + -0x36)) {
                  *(undefined4 *)(puVar9 + -4) = 0x5fa43;
                  FUN_0004c550();
                  puVar8 = extraout_ECX_03;
                }
                iVar7 = *(int *)(iVar5 + -0x3a);
                *(int *)(iVar5 + -0x3a) = iVar7 + 1;
                *(undefined4 *)(*piVar1 + iVar7 * 4) = *puVar8;
              }
              else {
                *(undefined4 *)(puVar9 + -4) = 0x5fa6f;
                iVar7 = FUN_00076d20();
                if (iVar7 == 0) {
                  *(undefined4 *)(puVar9 + 0x34) = 0;
                  *(undefined4 *)(puVar9 + 0x38) = 0;
                  *(undefined1 **)(puVar9 + -4) = puVar9 + 0x38;
                  *(undefined1 **)(puVar9 + -8) = puVar9 + 0x34;
                  *(char **)(puVar9 + -0xc) = s____d____d___0009303c;
                  *(undefined1 **)(puVar9 + -0x10) = puVar9;
                  *(undefined ***)(puVar9 + 0x3c) = &PTR_LAB_00098278;
                  *(undefined4 *)(puVar9 + -0x14) = 0x5fab2;
                  FUN_00049920();
                  *(undefined4 *)(puVar9 + -4) = 0x5fac3;
                  FUN_00057d30();
                  *(undefined1 **)(puVar9 + -4) = puVar9 + 0x10c;
                  *(char **)(puVar9 + -8) = s____d_00093051;
                  *(undefined1 **)(puVar9 + -0xc) = puVar9;
                  *(undefined4 *)(puVar9 + -0x10) = 0x5fada;
                  FUN_00049920();
                  *(undefined4 *)(puVar9 + -4) = 0x5fae7;
                  iVar7 = FUN_0005c320();
                  uVar6 = 0;
                  if (iVar7 != 0) {
                    *(undefined4 *)(puVar9 + -4) = *(undefined4 *)(puVar9 + 0x10c);
                    *(undefined4 *)(puVar9 + -8) = 0x5fb05;
                    uVar6 = FUN_0005f050();
                    puVar9 = puVar9 + -4;
                  }
                  *(undefined4 *)(puVar9 + 0x110) = uVar6;
                  puVar8 = (undefined4 *)(puVar9 + 0x110);
                  if (*(int *)(iVar5 + -0x3a) == *(int *)(iVar5 + -0x36)) {
                    *(undefined4 *)(puVar9 + -4) = 0x5fb29;
                    FUN_0004c550();
                    puVar8 = extraout_ECX_04;
                  }
                  iVar7 = *(int *)(iVar5 + -0x3a);
                  *(int *)(iVar5 + -0x3a) = iVar7 + 1;
                  *(undefined4 *)(iVar7 * 4 + *piVar1) = *puVar8;
                }
                else {
                  *(undefined4 *)(puVar9 + -4) = 0x5fb55;
                  iVar7 = FUN_00076d20();
                  if (iVar7 == 0) {
                    *(undefined4 *)(puVar9 + -4) = 0x5fb76;
                    FUN_00057d30();
                  }
                  else {
                    *(undefined4 *)(puVar9 + -4) = 0x5fb8c;
                    iVar7 = FUN_00076d20();
                    if (iVar7 == 0) {
                      *(undefined4 *)(puVar9 + -4) = 0x5fbb1;
                      FUN_00057d30();
                      *(undefined4 *)(puVar9 + -4) = 0x5fbcb;
                      FUN_00057a70();
                      *(undefined4 *)(puVar9 + -4) = 0x5fbd9;
                      FUN_00057a70();
                      *(undefined4 *)(puVar9 + 0x19) = 0;
                      *(undefined ***)(puVar9 + 0x21) = &PTR_FUN_000961fc;
                      *(undefined4 *)(puVar9 + 0x1d) = 0;
                      *(undefined4 *)(puVar9 + -4) = 0x5fbfc;
                      FUN_00049a40();
                      *(undefined4 *)(puVar9 + -4) = 0x5fc0d;
                      FUN_00057a00();
                      *(undefined4 *)(puVar9 + -4) = 0x5fc1e;
                      FUN_00057a00();
                      if (*(int **)(puVar9 + 0x19) != (int *)0x0) {
                        pcVar2 = *(code **)(**(int **)(puVar9 + 0x19) + 0x1c);
                        *(undefined4 *)(puVar9 + -4) = 0x5fc35;
                        (*pcVar2)();
                      }
                      *(undefined4 *)(puVar9 + -4) = 0x5fc3e;
                      FUN_00049af0();
                      *(undefined4 *)(puVar9 + -4) = 0x5fc49;
                      FUN_00057a00();
                    }
                    else {
                      *(undefined4 *)(puVar9 + -4) = 0x5fc5f;
                      iVar7 = FUN_00076d20();
                      if (iVar7 == 0) {
                        *(undefined4 *)(puVar9 + -4) = 0x5fc80;
                        FUN_00057d30();
                        if ((*(char **)(puVar9 + 0xe4) == (char *)0x0) ||
                           (**(char **)(puVar9 + 0xe4) == '\0')) {
                          bVar4 = true;
                        }
                        else {
                          bVar4 = false;
                        }
                        *(uint *)(iVar5 + -0x56) = (uint)!bVar4;
                      }
                      else {
                        *(undefined4 *)(puVar9 + -4) = 0x5fcc0;
                        iVar7 = FUN_00076d20();
                        if (iVar7 == 0) {
                          *(undefined4 *)(puVar9 + -4) = 0x5fce1;
                          FUN_00057d30();
                          *(undefined4 *)(puVar9 + -4) = 0x5fceb;
                          iVar7 = FUN_0005c320();
                          uVar6 = 0;
                          if (iVar7 != 0) {
                            *(undefined4 *)(puVar9 + -4) = 0x5fcfd;
                            uVar6 = FUN_00043e70();
                          }
                          *(undefined4 *)(iVar5 + -0x6a) = uVar6;
                          *(uint *)(iVar5 + -0x52) = DAT_000ceb84 & 1;
                        }
                        else {
                          *(undefined4 *)(puVar9 + -4) = 0x5fd21;
                          iVar7 = FUN_00076d20();
                          if (iVar7 == 0) {
                            *(undefined4 *)(puVar9 + -4) = *(undefined4 *)(puVar9 + 0x13c);
                            *(undefined4 *)(puVar9 + -8) = *(undefined4 *)(puVar9 + 0x130);
                            *(char **)(puVar9 + -0xc) = s____d____d_00093099;
                            *(undefined1 **)(puVar9 + -0x10) = puVar9;
                            *(undefined4 *)(puVar9 + -0x14) = 0x5fd4c;
                            FUN_00049920();
                            *(int *)(iVar5 + -0x4a) = *(int *)(iVar5 + -0x4a) * 0x46;
                          }
                          else {
                            *(undefined4 *)(puVar9 + -4) = 0x5fd6c;
                            iVar7 = FUN_00076d20();
                            if (iVar7 == 0) {
                              *(undefined ***)(puVar9 + 0x9c) = &PTR_LAB_00098278;
                              *(undefined1 **)(puVar9 + -4) = puVar9 + 0x98;
                              *(undefined1 **)(puVar9 + -8) = puVar9 + 0x94;
                              *(char **)(puVar9 + -0xc) = s____d____d___000930b8;
                              *(undefined1 **)(puVar9 + -0x10) = puVar9;
                              *(undefined4 *)(puVar9 + 0x94) = 0;
                              *(undefined4 *)(puVar9 + 0x98) = 0;
                              *(undefined4 *)(puVar9 + -0x14) = 0x5fdbd;
                              FUN_00049920();
                              *(undefined4 *)(puVar9 + -4) = 0x5fdce;
                              FUN_00057d30();
                              *(undefined4 *)(puVar9 + -4) = 0x5fdd8;
                              iVar7 = FUN_0005c320();
                              uVar6 = 0;
                              if (iVar7 != 0) {
                                *(undefined4 *)(puVar9 + -4) = 1;
                                *(undefined4 *)(puVar9 + -8) = 0x5fdf3;
                                uVar6 = FUN_0005f0b0();
                                puVar9 = puVar9 + -4;
                              }
                              *(undefined4 *)(iVar5 + 0x16) = uVar6;
                              *(undefined4 *)(puVar9 + 0x114) = uVar6;
                              puVar8 = (undefined4 *)(puVar9 + 0x114);
                              if (*(int *)(iVar5 + -0x3a) == *(int *)(iVar5 + -0x36)) {
                                *(undefined4 *)(puVar9 + -4) = 0x5fe1d;
                                FUN_0004c550();
                                puVar8 = extraout_ECX_05;
                              }
                              iVar7 = *(int *)(iVar5 + -0x3a);
                              *(int *)(iVar5 + -0x3a) = iVar7 + 1;
                              *(undefined4 *)(iVar7 * 4 + *piVar1) = *puVar8;
                            }
                            else {
                              *(undefined4 *)(puVar9 + -4) = 0x5fe49;
                              iVar7 = FUN_00076d20();
                              if (iVar7 == 0) {
                                *(undefined ***)(puVar9 + 0x60) = &PTR_LAB_00098278;
                                *(undefined1 **)(puVar9 + -4) = puVar9 + 0x5c;
                                *(undefined1 **)(puVar9 + -8) = puVar9 + 0x58;
                                *(char **)(puVar9 + -0xc) = s____d____d___000930df;
                                *(undefined1 **)(puVar9 + -0x10) = puVar9;
                                *(undefined4 *)(puVar9 + 0x58) = 0;
                                *(undefined4 *)(puVar9 + 0x5c) = 0;
                                *(undefined4 *)(puVar9 + -0x14) = 0x5fe8b;
                                FUN_00049920();
                                *(undefined4 *)(puVar9 + -4) = 0x5fe9c;
                                FUN_00057d30();
                                *(undefined4 *)(puVar9 + -4) = 0x5fea6;
                                iVar7 = FUN_0005c320();
                                uVar6 = 0;
                                if (iVar7 != 0) {
                                  *(undefined4 *)(puVar9 + -4) = 1;
                                  *(undefined4 *)(puVar9 + -8) = 0x5febe;
                                  uVar6 = FUN_0005f0b0();
                                  puVar9 = puVar9 + -4;
                                }
                                *(undefined4 *)(iVar5 + 0x1a) = uVar6;
                                *(undefined4 *)(puVar9 + 0x118) = uVar6;
                                puVar8 = (undefined4 *)(puVar9 + 0x118);
                                if (*(int *)(iVar5 + -0x3a) == *(int *)(iVar5 + -0x36)) {
                                  *(undefined4 *)(puVar9 + -4) = 0x5fee8;
                                  FUN_0004c550();
                                  puVar8 = extraout_ECX_06;
                                }
                                iVar7 = *(int *)(iVar5 + -0x3a);
                                *(int *)(iVar5 + -0x3a) = iVar7 + 1;
                                *(undefined4 *)(*piVar1 + iVar7 * 4) = *puVar8;
                              }
                              else {
                                *(undefined4 *)(puVar9 + -4) = 0x5ff14;
                                iVar7 = FUN_00076d20();
                                if (iVar7 == 0) {
                                  *(undefined ***)(puVar9 + 0x48) = &PTR_LAB_00098278;
                                  *(undefined1 **)(puVar9 + -4) = puVar9 + 0x44;
                                  *(undefined1 **)(puVar9 + -8) = puVar9 + 0x40;
                                  *(char **)(puVar9 + -0xc) = s____d____d___00093109;
                                  *(undefined1 **)(puVar9 + -0x10) = puVar9;
                                  *(undefined4 *)(puVar9 + 0x40) = 0;
                                  *(undefined4 *)(puVar9 + 0x44) = 0;
                                  *(undefined4 *)(puVar9 + -0x14) = 0x5ff56;
                                  FUN_00049920();
                                  *(undefined4 *)(puVar9 + -4) = 0x5ff67;
                                  FUN_00057d30();
                                  *(undefined1 **)(puVar9 + -4) = puVar9 + 0x11c;
                                  *(char **)(puVar9 + -8) = s____d_0009311e;
                                  *(undefined1 **)(puVar9 + -0xc) = puVar9;
                                  *(undefined4 *)(puVar9 + -0x10) = 0x5ff7e;
                                  FUN_00049920();
                                  *(undefined4 *)(puVar9 + -4) = 0x5ff8b;
                                  iVar7 = FUN_0005c320();
                                  uVar6 = 0;
                                  if (iVar7 != 0) {
                                    *(undefined4 *)(puVar9 + -4) = *(undefined4 *)(puVar9 + 0x11c);
                                    *(undefined4 *)(puVar9 + -8) = 0x5ffa9;
                                    uVar6 = FUN_0005eeb0();
                                    puVar9 = puVar9 + -4;
                                  }
                                  *(undefined4 *)(iVar5 + 0x1e) = uVar6;
                                  puVar8 = *(undefined4 **)(puVar9 + 0x140);
                                  if (*(int *)(iVar5 + -0x3a) == *(int *)(iVar5 + -0x36)) {
                                    *(undefined4 *)(puVar9 + -4) = 0x5ffcc;
                                    FUN_0004c550();
                                    puVar8 = extraout_ECX_07;
                                  }
                                  iVar7 = *(int *)(iVar5 + -0x3a);
                                  *(int *)(iVar5 + -0x3a) = iVar7 + 1;
                                  *(undefined4 *)(iVar7 * 4 + *piVar1) = *puVar8;
                                }
                                else {
                                  *(undefined4 *)(puVar9 + -4) = 0x5fff8;
                                  iVar7 = FUN_00076d20();
                                  if (iVar7 == 0) {
                                    *(undefined ***)(puVar9 + 0x90) = &PTR_LAB_00098278;
                                    *(undefined1 **)(puVar9 + -4) = puVar9 + 0x8c;
                                    *(undefined1 **)(puVar9 + -8) = puVar9 + 0x88;
                                    *(char **)(puVar9 + -0xc) = s____d____d___00093130;
                                    *(undefined1 **)(puVar9 + -0x10) = puVar9;
                                    *(undefined4 *)(puVar9 + 0x88) = 0;
                                    *(undefined4 *)(puVar9 + 0x8c) = 0;
                                    *(undefined4 *)(puVar9 + -0x14) = 0x60049;
                                    FUN_00049920();
                                    *(undefined4 *)(puVar9 + -4) = 0x6005a;
                                    FUN_00057d30();
                                    *(undefined1 **)(puVar9 + -4) = puVar9 + 0x120;
                                    *(char **)(puVar9 + -8) = s____d_00093145;
                                    *(undefined1 **)(puVar9 + -0xc) = puVar9;
                                    *(undefined4 *)(puVar9 + -0x10) = 0x60071;
                                    FUN_00049920();
                                    *(undefined4 *)(puVar9 + -4) = 0x6007e;
                                    iVar7 = FUN_0005c320();
                                    uVar6 = 0;
                                    if (iVar7 != 0) {
                                      *(undefined4 *)(puVar9 + -4) = *(undefined4 *)(puVar9 + 0x120)
                                      ;
                                      *(undefined4 *)(puVar9 + -8) = 0x6009f;
                                      uVar6 = FUN_0005f150();
                                      puVar9 = puVar9 + -4;
                                    }
                                    *(undefined4 *)(iVar5 + 0x22) = uVar6;
                                    *(undefined4 *)(puVar9 + 0x124) = uVar6;
                                    puVar8 = (undefined4 *)(puVar9 + 0x124);
                                    if (*(int *)(iVar5 + -0x3a) == *(int *)(iVar5 + -0x36)) {
                                      *(undefined4 *)(puVar9 + -4) = 0x600c9;
                                      FUN_0004c550();
                                      puVar8 = extraout_ECX_08;
                                    }
                                    iVar7 = *(int *)(iVar5 + -0x3a);
                                    *(int *)(iVar5 + -0x3a) = iVar7 + 1;
                                    *(undefined4 *)(iVar7 * 4 + *piVar1) = *puVar8;
                                  }
                                  else {
                                    *(undefined4 *)(puVar9 + -4) = 0x600f5;
                                    iVar7 = FUN_00076d20();
                                    if (iVar7 == 0) {
                                      *(undefined4 *)(puVar9 + 0x7c) = 0;
                                      *(undefined4 *)(puVar9 + 0x80) = 0;
                                      *(undefined1 **)(puVar9 + -4) = puVar9 + 0x80;
                                      *(undefined1 **)(puVar9 + -8) = puVar9 + 0x7c;
                                      *(char **)(puVar9 + -0xc) = s____d____d___00093155;
                                      *(undefined1 **)(puVar9 + -0x10) = puVar9;
                                      *(undefined ***)(puVar9 + 0x84) = &PTR_LAB_00098278;
                                      *(undefined4 *)(puVar9 + -0x14) = 0x60144;
                                      FUN_00049920();
                                      *(undefined4 *)(puVar9 + -4) = 0x60155;
                                      FUN_00057d30();
                                      *(undefined1 **)(puVar9 + -4) = puVar9 + 0x128;
                                      *(char **)(puVar9 + -8) = s____d_0009316a;
                                      *(undefined1 **)(puVar9 + -0xc) = puVar9;
                                      *(undefined4 *)(puVar9 + -0x10) = 0x6016c;
                                      FUN_00049920();
                                      *(undefined4 *)(puVar9 + -4) = 0x60179;
                                      iVar7 = FUN_0005c320();
                                      uVar6 = 0;
                                      if (iVar7 != 0) {
                                        *(undefined4 *)(puVar9 + -4) =
                                             *(undefined4 *)(puVar9 + 0x128);
                                        *(undefined4 *)(puVar9 + -8) = 0x60197;
                                        uVar6 = FUN_0005f150();
                                        puVar9 = puVar9 + -4;
                                      }
                                      *(undefined4 *)(iVar5 + 0x26) = uVar6;
                                      *(undefined4 *)(puVar9 + 300) = uVar6;
                                      puVar8 = (undefined4 *)(puVar9 + 300);
                                      if (*(int *)(iVar5 + -0x3a) == *(int *)(iVar5 + -0x36)) {
                                        *(undefined4 *)(puVar9 + -4) = 0x601c1;
                                        FUN_0004c550();
                                        puVar8 = extraout_ECX_09;
                                      }
                                      iVar7 = *(int *)(iVar5 + -0x3a);
                                      *(int *)(iVar5 + -0x3a) = iVar7 + 1;
                                      *(undefined4 *)(iVar7 * 4 + *piVar1) = *puVar8;
                                    }
                                    else {
                                      *(undefined4 *)(puVar9 + -4) = 0x601ed;
                                      iVar7 = FUN_00076d20();
                                      if (iVar7 == 0) {
                                        *(undefined1 **)(puVar9 + -4) = puVar9 + 0x68;
                                        *(undefined1 **)(puVar9 + -8) = puVar9 + 100;
                                        *(char **)(puVar9 + -0xc) = s____d____d___00093180;
                                        *(undefined4 *)(puVar9 + 100) = 0;
                                        *(undefined1 **)(puVar9 + -0x10) = puVar9;
                                        *(undefined4 *)(puVar9 + 0x68) = 0;
                                        *(undefined ***)(puVar9 + 0x6c) = &PTR_LAB_00098278;
                                        *(undefined4 *)(puVar9 + -0x14) = 0x6022d;
                                        FUN_00049920();
                                        *(undefined4 *)(puVar9 + -4) = 0x60241;
                                        FUN_00057d30();
                                        *(undefined4 *)(puVar9 + -4) = 0x6024b;
                                        iVar7 = FUN_0005c320();
                                        uVar6 = 0;
                                        if (iVar7 != 0) {
                                          *(undefined4 *)(puVar9 + -4) = 1;
                                          *(undefined4 *)(puVar9 + -8) = 0x60263;
                                          uVar6 = FUN_0005ed40();
                                          puVar9 = puVar9 + -4;
                                        }
                                        *(undefined4 *)(iVar5 + 0x2a) = uVar6;
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  } while( true );
}

