
void __regparm3 FUN_0005f190(int *param_1)

{
  char cVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 *extraout_ECX;
  undefined4 *unaff_EBX;
  char *pcVar4;
  char *pcVar5;
  undefined8 uVar6;
  
  puVar2 = (undefined4 *)thunk_FUN_0005c320();
  uVar6 = thunk_FUN_0005c320();
  *(undefined4 *)((ulonglong)uVar6 >> 0x20) = (int)uVar6;
  uVar6 = thunk_FUN_0005c320();
  *(int *)((int)((ulonglong)uVar6 >> 0x20) + 4) = (int)uVar6;
  iVar3 = FUN_0005c320();
  if (iVar3 != 0) {
    *(undefined4 *)(iVar3 + 0x10) = 0;
    *(undefined4 *)(iVar3 + 0x14) = 0;
    *(undefined4 *)(iVar3 + 0x18) = 0;
    *(undefined4 *)(iVar3 + 0x1c) = 0;
    *(undefined4 *)(iVar3 + 0x20) = 0;
    *(undefined4 *)(iVar3 + 0x24) = 0;
    *(undefined4 *)(iVar3 + 0x28) = 0;
    *(undefined4 *)(iVar3 + 0x2c) = 0;
    *(undefined4 *)(iVar3 + 0x30) = 0;
    *(undefined4 *)(iVar3 + 0x34) = 3;
    *(undefined4 *)(iVar3 + 0x38) = 3;
    *(undefined4 *)(iVar3 + 0x3c) = 0x32;
    *(undefined4 *)(iVar3 + 0x40) = 0x32;
    *(undefined4 *)(iVar3 + 0x44) = 0;
  }
  *param_1 = iVar3;
  pcVar4 = (char *)*unaff_EBX;
  pcVar5 = (char *)*puVar2;
  do {
    cVar1 = *pcVar4;
    *pcVar5 = cVar1;
    if (cVar1 == '\0') break;
    cVar1 = pcVar4[1];
    pcVar4 = pcVar4 + 2;
    pcVar5[1] = cVar1;
    pcVar5 = pcVar5 + 2;
  } while (cVar1 != '\0');
  if (param_1[6] == 0) {
    FUN_0005c6b0();
    FUN_00057a70();
    FUN_00057250();
  }
  else {
    pcVar4 = (char *)*extraout_ECX;
    pcVar5 = (char *)puVar2[1];
    do {
      cVar1 = *pcVar4;
      *pcVar5 = cVar1;
      if (cVar1 == '\0') break;
      cVar1 = pcVar4[1];
      pcVar4 = pcVar4 + 2;
      pcVar5[1] = cVar1;
      pcVar5 = pcVar5 + 2;
    } while (cVar1 != '\0');
    FUN_0005c6b0();
    FUN_00057a70();
    FUN_00057250();
  }
  FUN_00057a00();
  FUN_0005c370();
  FUN_0005c370();
  FUN_0005c370();
  return;
}

