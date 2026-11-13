
void __regparm3 FUN_00051913(int param_1)

{
  bool bVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 *extraout_ECX;
  int *extraout_ECX_00;
  int *piVar4;
  int iVar5;
  int unaff_EBX;
  char **ppcVar6;
  undefined4 *puVar7;
  byte bVar8;
  char *pcStack_40;
  undefined1 local_3c [24];
  undefined4 local_24;
  undefined **local_20;
  int local_1c;
  char local_18 [4];
  undefined4 local_14;
  undefined4 local_10;
  undefined4 *local_c;
  char local_8 [4];
  
  bVar8 = 0;
  local_24 = 0;
  local_20 = &PTR_FUN_000961fc;
  pcStack_40 = (char *)0x51938;
  iVar2 = FUN_00076d20();
  if (iVar2 == 0) {
    pcStack_40 = (char *)(param_1 + 8);
    FUN_00049920();
    pcStack_40 = (char *)(param_1 + 10);
    FUN_000498f0();
  }
  else {
    pcStack_40 = (char *)0x51979;
    iVar2 = FUN_00076d20();
    if (iVar2 == 0) {
      pcStack_40 = (char *)(param_1 + 0x16);
      FUN_00049920();
      pcStack_40 = (char *)0x519a4;
      FUN_00057a00();
      return;
    }
    pcStack_40 = (char *)0x519b7;
    iVar2 = FUN_00076d20();
    if (iVar2 == 0) {
      pcStack_40 = (char *)(param_1 + 0xe);
      FUN_00049920();
      pcStack_40 = (char *)0x519e2;
      FUN_00057a00();
      return;
    }
    pcStack_40 = (char *)0x519f5;
    iVar2 = FUN_00076d20();
    if (iVar2 == 0) {
      local_1c = *(int *)(*(int *)(DAT_000cef40 + 0x398) + 0x16);
      pcStack_40 = (char *)0x51a24;
      FUN_00057d30();
      pcStack_40 = (char *)0x51a35;
      DAT_000ceff4 = (**(code **)(local_1c + 4))();
      pcStack_40 = (char *)0x51a47;
      FUN_00057a00();
      return;
    }
    pcStack_40 = (char *)0x51a5a;
    iVar2 = FUN_00076d20();
    if (iVar2 == 0) {
      pcStack_40 = (char *)0x51a7b;
      FUN_00057d30();
      pcStack_40 = (char *)0x51a95;
      (**(code **)(*(int *)(*(int *)(DAT_000cef40 + 0x398) + 0x16) + 4))();
      pcStack_40 = local_18;
      FUN_00049920();
      pcStack_40 = (char *)0x51ab3;
      iVar2 = FUN_0005c320();
      ppcVar6 = (char **)local_3c;
      local_14 = 0;
      if (iVar2 != 0) {
        pcStack_40 = (char *)0x3e8;
        local_14 = FUN_000515e0();
        ppcVar6 = &pcStack_40;
      }
      puVar3 = &local_14;
      if (*(int *)(param_1 + 0x36) == *(int *)(param_1 + 0x3a)) {
        *(undefined4 *)((int)ppcVar6 + -4) = 0x51ae3;
        FUN_00021e80();
        puVar3 = extraout_ECX;
      }
      iVar2 = *(int *)(param_1 + 0x36);
      *(int *)(param_1 + 0x36) = iVar2 + 1;
      *(undefined4 *)(iVar2 * 4 + *(int *)(param_1 + 0x2e)) = *puVar3;
      *(undefined4 *)((int)ppcVar6 + -4) = 0x51b06;
      FUN_00057a00();
      return;
    }
    pcStack_40 = (char *)0x51b19;
    iVar2 = FUN_00076d20();
    if (iVar2 == 0) {
      local_c = (undefined4 *)(unaff_EBX + 5);
      *(undefined4 *)(param_1 + 0x12) = 0;
      do {
        pcStack_40 = (char *)0x51b49;
        FUN_00057d30();
        pcStack_40 = (char *)0x51b56;
        iVar2 = FUN_00076a69();
        if (iVar2 != 0) {
          pcStack_40 = (char *)0x51b6b;
          iVar2 = FUN_00076a69();
          if (iVar2 == 0) {
            *(byte *)(param_1 + 0x12) = *(byte *)(param_1 + 0x12) | 1;
          }
          else {
            pcStack_40 = (char *)0x51b85;
            iVar2 = FUN_00076a69();
            if (iVar2 == 0) {
              *(byte *)(param_1 + 0x12) = *(byte *)(param_1 + 0x12) | 2;
            }
            else {
              pcStack_40 = (char *)0x51b9f;
              iVar2 = FUN_00076a69();
              if (iVar2 == 0) {
                *(byte *)(param_1 + 0x12) = *(byte *)(param_1 + 0x12) | 3;
              }
              else {
                pcStack_40 = (char *)0x51bb9;
                iVar2 = FUN_00076a69();
                if (iVar2 == 0) {
                  *(byte *)(param_1 + 0x12) = *(byte *)(param_1 + 0x12) | 4;
                }
                else {
                  pcStack_40 = (char *)0x51bd3;
                  iVar2 = FUN_00076a69();
                  if (iVar2 == 0) {
                    *(byte *)(param_1 + 0x12) = *(byte *)(param_1 + 0x12) | 8;
                  }
                  else {
                    pcStack_40 = (char *)0x51bed;
                    iVar2 = FUN_00076a69();
                    if (iVar2 == 0) {
                      *(byte *)(param_1 + 0x12) = *(byte *)(param_1 + 0x12) | 0x10;
                    }
                    else {
                      pcStack_40 = (char *)0x51c07;
                      iVar2 = FUN_00076a69();
                      if (iVar2 == 0) {
                        *(byte *)(param_1 + 0x12) = *(byte *)(param_1 + 0x12) | 0x20;
                      }
                      else {
                        pcStack_40 = (char *)0x51c21;
                        iVar2 = FUN_00076a69();
                        if (iVar2 == 0) {
                          *(byte *)(param_1 + 0x12) = *(byte *)(param_1 + 0x12) | 0x80;
                        }
                        else {
                          pcStack_40 = (char *)0x51c3b;
                          iVar2 = FUN_00076a69();
                          if (iVar2 == 0) {
                            *(byte *)(param_1 + 0x12) = *(byte *)(param_1 + 0x12) | 0x40;
                          }
                          else {
                            pcStack_40 = (char *)0x51c55;
                            iVar2 = FUN_00076a69();
                            if (iVar2 == 0) {
                              *(byte *)(param_1 + 0x13) = *(byte *)(param_1 + 0x13) | 1;
                            }
                            else {
                              pcStack_40 = (char *)0x51c6f;
                              iVar2 = FUN_00076a69();
                              if (iVar2 == 0) {
                                *(ushort *)(param_1 + 0x12) = *(ushort *)(param_1 + 0x12) | 0x1c0;
                              }
                              else {
                                pcStack_40 = (char *)0x51c8b;
                                iVar2 = FUN_00076a69();
                                if (iVar2 == 0) {
                                  *(byte *)(param_1 + 0x13) = *(byte *)(param_1 + 0x13) | 2;
                                }
                                else {
                                  pcStack_40 = (char *)0x51ca5;
                                  iVar2 = FUN_00076a69();
                                  if (iVar2 == 0) {
                                    *(byte *)(param_1 + 0x13) = *(byte *)(param_1 + 0x13) | 0x40;
                                  }
                                  else {
                                    pcStack_40 = (char *)0x51cbf;
                                    iVar2 = FUN_00076a69();
                                    if (iVar2 == 0) {
                                      *(byte *)(param_1 + 0x13) = *(byte *)(param_1 + 0x13) | 4;
                                    }
                                    else {
                                      pcStack_40 = (char *)0x51cd9;
                                      iVar2 = FUN_00076a69();
                                      if (iVar2 == 0) {
                                        *(byte *)(param_1 + 0x13) = *(byte *)(param_1 + 0x13) | 8;
                                      }
                                      else {
                                        pcStack_40 = (char *)0x51cf3;
                                        iVar2 = FUN_00076a69();
                                        if (iVar2 == 0) {
                                          *(byte *)(param_1 + 0x13) =
                                               *(byte *)(param_1 + 0x13) | 0x10;
                                        }
                                        else {
                                          pcStack_40 = (char *)0x51d0d;
                                          iVar2 = FUN_00076a69();
                                          if (iVar2 == 0) {
                                            *(byte *)(param_1 + 0x13) =
                                                 *(byte *)(param_1 + 0x13) | 0x20;
                                          }
                                          else {
                                            pcStack_40 = (char *)0x51d27;
                                            iVar2 = FUN_00076a69();
                                            if (iVar2 == 0) {
                                              *(byte *)(param_1 + 0x13) =
                                                   *(byte *)(param_1 + 0x13) | 0x80;
                                            }
                                            else {
                                              pcStack_40 = (char *)0x51d41;
                                              iVar2 = FUN_00076a69();
                                              if (iVar2 == 0) {
                                                *(byte *)(param_1 + 0x14) =
                                                     *(byte *)(param_1 + 0x14) | 1;
                                              }
                                              else {
                                                pcStack_40 = (char *)0x51d5b;
                                                iVar2 = FUN_00076a69();
                                                if (iVar2 == 0) {
                                                  *(uint *)(param_1 + 0x12) =
                                                       *(uint *)(param_1 + 0x12) | 0x20400;
                                                }
                                                else {
                                                  pcStack_40 = (char *)0x51d75;
                                                  iVar2 = FUN_00076a69();
                                                  if (iVar2 == 0) {
                                                    *(uint *)(param_1 + 0x12) =
                                                         *(uint *)(param_1 + 0x12) | 0x40400;
                                                  }
                                                  else {
                                                    pcStack_40 = (char *)0x51d8f;
                                                    iVar2 = FUN_00076a69();
                                                    if (iVar2 == 0) {
                                                      *(byte *)(param_1 + 0x14) =
                                                           *(byte *)(param_1 + 0x14) | 8;
                                                    }
                                                    else {
                                                      pcStack_40 = (char *)*local_c;
                                                      FUN_000496b0(&DAT_000a92d4,
                                                                                                                                      
                                                  s_Bledny_control__s_w_pliku__s__0009223b,local_24)
                                                  ;
                                                  pcStack_40 = (char *)0x51dc3;
                                                  ll_rethrow_report();
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
                }
              }
            }
          }
        }
        pcStack_40 = local_8;
        FUN_00049920();
        if ((local_8[0] == ',') || (local_8[0] == '|')) {
          bVar1 = true;
        }
        else {
          bVar1 = false;
        }
      } while (bVar1);
      pcStack_40 = (char *)0x51e00;
      FUN_00057a00();
      return;
    }
    pcStack_40 = (char *)0x51e10;
    local_10 = FUN_00053440();
    pcStack_40 = (char *)0x51e27;
    FUN_00057d30();
    pcStack_40 = (char *)0x51e2f;
    iVar2 = FUN_00057db0();
    if (iVar2 != 0) {
      pcStack_40 = (char *)0x51e3b;
      FUN_00057fd0();
    }
    pcStack_40 = (char *)0x51e52;
    puVar3 = (undefined4 *)FUN_00049610();
    piVar4 = (int *)(param_1 + 0x1a);
    if (*(int *)(param_1 + 0x22) == *(int *)(param_1 + 0x26)) {
      pcStack_40 = (char *)0x51e6b;
      FUN_00022010();
      piVar4 = extraout_ECX_00;
    }
    iVar2 = piVar4[2];
    piVar4[2] = iVar2 + 1;
    puVar7 = (undefined4 *)(*piVar4 + iVar2 * 0x15);
    for (iVar5 = 5; iVar5 != 0; iVar5 = iVar5 + -1) {
      *puVar7 = *puVar3;
      puVar3 = puVar3 + (uint)bVar8 * -2 + 1;
      puVar7 = puVar7 + (uint)bVar8 * -2 + 1;
    }
    *(undefined1 *)puVar7 = *(undefined1 *)puVar3;
  }
  pcStack_40 = (char *)0x51e9f;
  FUN_00057a00();
  return;
}

