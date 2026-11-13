
int __regparm3 FUN_00077930(char *param_1,int param_2)

{
  undefined1 *puVar1;
  int extraout_ECX;
  int *extraout_ECX_00;
  int *piVar2;
  int extraout_ECX_01;
  char *pcVar3;
  int iVar4;
  int iVar5;
  undefined8 uVar6;
  
  piVar2 = DAT_000a9bb4;
  do {
    if (*piVar2 == 0) {
      return (int)DAT_000a9bb4 - (int)piVar2 >> 2;
    }
    for (pcVar3 = param_1; *pcVar3 != '\0'; pcVar3 = pcVar3 + 1) {
      FUN_00076e98();
      uVar6 = FUN_00076e98();
      if (extraout_ECX != (int)uVar6) break;
      if (*(char *)((ulonglong)uVar6 >> 0x20) == '=') {
        iVar5 = (int)piVar2 - (int)DAT_000a9bb4 >> 2;
        if (param_2 == 0) {
          return iVar5 + 1;
        }
        for (; *piVar2 != 0; piVar2 = piVar2 + 1) {
          *piVar2 = piVar2[1];
        }
        if (DAT_000a9bb8 != 0) {
          if (*(char *)(iVar5 + DAT_000a9bb8) != '\0') {
            FUN_00075302();
            piVar2 = extraout_ECX_00;
          }
          iVar4 = (int)piVar2 - (int)DAT_000a9bb4;
          FUN_0007bb8f();
          puVar1 = (undefined1 *)(iVar5 + extraout_ECX_01);
          DAT_000a9bb8 = extraout_ECX_01;
          for (; iVar5 < iVar4 >> 2; iVar5 = iVar5 + 1) {
            *puVar1 = puVar1[1];
            puVar1 = puVar1 + 1;
          }
        }
        return 0;
      }
    }
    piVar2 = piVar2 + 1;
  } while( true );
}

