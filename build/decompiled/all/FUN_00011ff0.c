
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

longlong __regparm3 FUN_00011ff0(undefined4 param_1,uint param_2,undefined4 param_3)

{
  byte bVar1;
  byte *pbVar2;
  int iVar3;
  undefined4 uVar4;
  bool bVar5;
  
  DAT_000ae658 = 0xff;
  DAT_000ae659 = -1;
  DAT_000ae65a = 0xff;
  DAT_000ae644 = -1;
  pbVar2 = (byte *)FUN_00074c13(param_2,param_3);
  uVar4 = 0;
  if (pbVar2 != (byte *)0x0) {
    while( true ) {
      do {
        do {
          bVar1 = *pbVar2;
          pbVar2 = pbVar2 + 1;
        } while (bVar1 == 0x20);
      } while (bVar1 == 9);
      if (bVar1 == 0) break;
      if (bVar1 < 0x49) {
        if (bVar1 < 0x44) {
          bVar5 = bVar1 == 0x41;
LAB_000120ba:
          if (!bVar5) goto LAB_000120bc;
          DAT_000ae644 = FUN_00075136();
        }
        else if (bVar1 < 0x45) {
LAB_00012091:
          DAT_000ae659 = FUN_00075136();
        }
        else {
          if (bVar1 != 0x48) goto LAB_000120bc;
LAB_000120a4:
          DAT_000ae65a = FUN_00075136();
        }
      }
      else if (bVar1 < 0x4a) {
LAB_0001207e:
        DAT_000ae658 = FUN_00075136();
      }
      else {
        if (bVar1 < 100) {
          bVar5 = bVar1 == 0x61;
          goto LAB_000120ba;
        }
        if (bVar1 < 0x65) goto LAB_00012091;
        if (0x67 < bVar1) {
          if (bVar1 < 0x69) goto LAB_000120a4;
          if (bVar1 == 0x69) goto LAB_0001207e;
        }
LAB_000120bc:
        FUN_00075136();
      }
      pbVar2 = DAT_000ae640;
    }
    if (((DAT_000ae644 == -1) || (DAT_000ae658 == 0xff)) || (DAT_000ae659 == -1)) {
LAB_00012110:
      return (ulonglong)param_2 << 0x20;
    }
    if (DAT_000ae658 < 8) {
      _DAT_000ae654 = DAT_000ae658 + 8;
    }
    else {
      _DAT_000ae654 = DAT_000ae658 + 0x68;
    }
    iVar3 = FUN_00011fa4();
    uVar4 = 0;
    if (iVar3 != 0) {
      DAT_000ae656 = FUN_00011fc4();
      if (DAT_000ae656 == -1) goto LAB_00012110;
      uVar4 = 1;
    }
  }
  return CONCAT44(param_2,uVar4);
}

