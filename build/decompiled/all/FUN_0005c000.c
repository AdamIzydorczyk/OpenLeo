
void FUN_0005c000(void)

{
  char cVar1;
  short sVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  undefined4 *extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_EDX;
  char *pcVar6;
  char *pcVar7;
  
  pcVar6 = s_ULTRASND__00092474;
  pcVar7 = &DAT_000cf258;
  do {
    cVar1 = *pcVar6;
    *pcVar7 = cVar1;
    if (cVar1 == '\0') break;
    cVar1 = pcVar6[1];
    pcVar6 = pcVar6 + 2;
    pcVar7[1] = cVar1;
    pcVar7 = pcVar7 + 2;
  } while (cVar1 != '\0');
  FUN_000777c4();
  if ('\0' < *(char *)(extraout_ECX + 1)) {
    if (DAT_000a97a2 == 0) {
      if (*(char *)(extraout_ECX + 1) < '\x03') {
        DAT_000a97a4 = 7;
      }
      DAT_000a97a2 = (short)*extraout_ECX;
    }
    if (*(char *)(extraout_ECX + 1) < '\x05') {
      FUN_0007697a(&DAT_000cf28c,s_BLASTER_A_x_I_d_D_d_00092496,(int)DAT_000a97a2,(int)DAT_000a97a4,
                   (int)DAT_000a97a6);
      FUN_000777c4();
      return;
    }
    iVar4 = FUN_00040920();
    if (iVar4 == -1) {
      (**(code **)**(undefined4 **)(extraout_ECX_00 + 5))();
    }
    iVar4 = FUN_00040970();
    iVar5 = FUN_000409b0();
    if ((iVar4 == -1) || (iVar5 == -1)) {
      (**(code **)(**(int **)(extraout_ECX_01 + 5) + 4))();
    }
    sVar2 = FUN_00040920();
    FUN_00040970();
    sVar3 = FUN_000409b0();
    if (sVar2 != -1) {
      DAT_000a97a4 = sVar2;
    }
    if (extraout_EDX != -1) {
      DAT_000a97a6 = (short)extraout_EDX;
    }
    if (sVar3 == -1) {
      if ((0 < DAT_000cf2c6) && (DAT_000cf2c6 < 5)) {
        DAT_000a97a8 = DAT_000cf2c6;
      }
    }
    else {
      DAT_000a97a8 = sVar3;
    }
    FUN_0007697a(&DAT_000cf28c,s_BLASTER_A_x_I_d_D_d_H_d_0009247e,(int)DAT_000a97a2,
                 (int)DAT_000a97a4,(int)DAT_000a97a6,(int)DAT_000a97a8);
    FUN_000777c4();
    return;
  }
  pcVar6 = s_BLASTER__000924aa;
  pcVar7 = &DAT_000cf28c;
  do {
    cVar1 = *pcVar6;
    *pcVar7 = cVar1;
    if (cVar1 == '\0') break;
    cVar1 = pcVar6[1];
    pcVar6 = pcVar6 + 2;
    pcVar7[1] = cVar1;
    pcVar7 = pcVar7 + 2;
  } while (cVar1 != '\0');
  FUN_000777c4();
  return;
}

