
uint __regparm3 FUN_00074fc9(char *param_1,undefined4 *param_2,int param_3)

{
  undefined2 uVar1;
  char extraout_CH;
  uint extraout_ECX;
  char *pcVar2;
  char *pcVar3;
  int unaff_EBX;
  uint uVar4;
  undefined8 uVar5;
  
  if (param_2 != (undefined4 *)0x0) {
    *param_2 = param_1;
  }
  pcVar2 = param_1;
  while (((&DAT_000a68b4)[(byte)(*pcVar2 + 1)] & 2) != 0) {
    pcVar2 = pcVar2 + 1;
  }
  if ((*pcVar2 == '+') || (*pcVar2 == '-')) {
    pcVar2 = pcVar2 + 1;
  }
  if (unaff_EBX == 0) {
    if ((*pcVar2 == '0') && ((pcVar2[1] == 'x' || (pcVar2[1] == 'X')))) {
      unaff_EBX = 0x10;
    }
    else if (*pcVar2 == '0') {
      unaff_EBX = 8;
    }
    else {
      unaff_EBX = 10;
    }
  }
  if ((unaff_EBX < 2) || (0x24 < unaff_EBX)) {
    FUN_00078c12();
    uVar4 = 0;
  }
  else {
    if (((unaff_EBX == 0x10) && (*pcVar2 == '0')) && ((pcVar2[1] == 'x' || (pcVar2[1] == 'X')))) {
      pcVar2 = pcVar2 + 2;
    }
    uVar4 = 0;
    while( true ) {
      uVar5 = FUN_0007513e();
      pcVar3 = (char *)((ulonglong)uVar5 >> 0x20);
      uVar1 = (undefined2)extraout_ECX;
      if (unaff_EBX <= (int)uVar5) break;
      uVar4 = uVar4 * unaff_EBX + (int)uVar5;
    }
    if (pcVar3 == pcVar2) {
      pcVar3 = param_1;
    }
    if (param_2 != (undefined4 *)0x0) {
      *param_2 = pcVar3;
    }
    if (((param_3 == 1) && (0x7fffffff < uVar4)) &&
       ((uVar4 != 0x80000000 || ((extraout_ECX >> 8 & 0xff) != 0x2d)))) {
      uVar1 = (undefined2)CONCAT31((int3)(extraout_ECX >> 8),1);
    }
    if ((char)uVar1 == '\0') {
      if ((char)((ushort)uVar1 >> 8) == '-') {
        uVar4 = -uVar4;
      }
    }
    else {
      FUN_00078c12();
      if (param_3 == 0) {
        uVar4 = 0xffffffff;
      }
      else if (extraout_CH == '-') {
        uVar4 = 0x80000000;
      }
      else {
        uVar4 = 0x7fffffff;
      }
    }
  }
  return uVar4;
}

