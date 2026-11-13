
undefined8 __regparm3 FUN_00057f70(undefined4 param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  char *pcVar3;
  int iVar4;
  byte bVar5;
  
  bVar5 = 0;
  iVar4 = 0x31;
  do {
    iVar2 = FUN_00057e30();
    if (iVar2 != -1) {
      pcVar3 = (char *)FUN_00010598(param_1);
      if (pcVar3 != (char *)0x0) {
        iVar2 = -1;
        do {
          if (iVar2 == 0) break;
          iVar2 = iVar2 + -1;
          cVar1 = *pcVar3;
          pcVar3 = pcVar3 + (uint)bVar5 * -2 + 1;
        } while (cVar1 != '\0');
      }
      FUN_00057e70();
    }
    iVar4 = iVar4 + 1;
    if (0x39 < iVar4) {
      return CONCAT44(param_2,param_1);
    }
  } while( true );
}

