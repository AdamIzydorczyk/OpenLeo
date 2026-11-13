
void FUN_00011779(void)

{
  ushort uVar1;
  int extraout_ECX;
  char *extraout_ECX_00;
  ushort uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  byte local_c;
  byte local_8;
  
  uVar2 = 0;
  if (DAT_000ae63d < DAT_000ae63c) {
    DAT_000ae63d = DAT_000ae63d + 1;
  }
  else {
    DAT_000ae63d = '\0';
    (*(code *)PTR_FUN_000a71b4)();
    if (DAT_000ae63e != DAT_000a71b0) {
      FUN_00011720();
      DAT_000ae63e = DAT_000a71b0;
    }
    for (local_8 = 0; local_8 < DAT_000ae674; local_8 = local_8 + 1) {
      FUN_000112a4();
      if ((&DAT_000ae2bc)[extraout_ECX * 0x1c] != '\0') {
        uVar1 = FUN_000114d4();
        if (uVar2 <= uVar1) {
          uVar2 = uVar1;
        }
        FUN_000115d8();
      }
    }
    for (local_c = 0; local_c < DAT_000ae674; local_c = local_c + 1) {
      FUN_000112a4();
      if (*extraout_ECX_00 == '\0') {
        FUN_000112b8();
        FUN_000115d8();
        FUN_000114a0();
      }
      else {
        *extraout_ECX_00 = '\0';
        iVar5 = *(int *)(extraout_ECX_00 + 0xe);
        iVar4 = *(int *)(extraout_ECX_00 + 0x12);
        iVar3 = *(int *)(extraout_ECX_00 + 10);
        if ((extraout_ECX_00[2] & 1U) != 0) {
          iVar3 = iVar3 * 2;
          iVar5 = iVar5 * 2;
          iVar4 = iVar4 * 2;
        }
        FUN_00011430(iVar3,iVar4,iVar5);
        FUN_000112b8();
        FUN_000115d8();
        FUN_000114a0();
        FUN_00011418();
      }
    }
  }
  return;
}

