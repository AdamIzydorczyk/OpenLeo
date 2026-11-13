
void __regparm3 FUN_0007b166(undefined1 *param_1,uint param_2)

{
  int iVar1;
  char *extraout_ECX;
  char *pcVar2;
  char *pcVar3;
  uint uVar4;
  int unaff_EBX;
  undefined4 local_10;
  
  local_10 = param_2;
  if ((int)param_2 < 0) {
    local_10 = -param_2;
    *param_1 = 0x2d;
  }
  if (*(int *)(unaff_EBX + 8) == -1) {
    *(undefined4 *)(unaff_EBX + 8) = 4;
  }
  FUN_00076c9c();
  pcVar2 = extraout_ECX;
  while (pcVar3 = pcVar2 + 1, *pcVar2 != '\0') {
    pcVar2 = pcVar3;
  }
  if (*(int *)(unaff_EBX + 8) != 0) {
    *pcVar2 = '.';
    for (iVar1 = 0; iVar1 < *(int *)(unaff_EBX + 8); iVar1 = iVar1 + 1) {
      local_10 = local_10 & 0xffff;
      uVar4 = local_10 * 10;
      local_10._2_1_ = (char)(uVar4 >> 0x10);
      *pcVar3 = local_10._2_1_ + '0';
      pcVar3 = pcVar3 + 1;
      local_10 = uVar4;
    }
    *pcVar3 = '\0';
    pcVar2 = pcVar3;
  }
  if ((local_10 & 0x8000) != 0) {
    while (pcVar2 != extraout_ECX) {
      pcVar3 = pcVar2 + -1;
      if (*pcVar3 == '.') {
        pcVar3 = pcVar2 + -2;
      }
      if (*pcVar3 != '9') {
        *pcVar3 = *pcVar3 + '\x01';
        return;
      }
      *pcVar3 = '0';
      pcVar2 = pcVar3;
    }
    *extraout_ECX = '1';
    pcVar2 = extraout_ECX + 1;
    while( true ) {
      if (*pcVar2 != '0') break;
      pcVar2 = pcVar2 + 1;
    }
    if (*pcVar2 == '.') {
      *pcVar2 = '0';
      pcVar2[1] = '.';
      for (pcVar2 = pcVar2 + 2; *pcVar2 == '0'; pcVar2 = pcVar2 + 1) {
      }
    }
    *pcVar2 = '0';
    pcVar2[1] = '\0';
  }
  return;
}

