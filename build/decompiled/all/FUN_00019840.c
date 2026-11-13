
longlong FUN_00019840(void)

{
  byte bVar1;
  char cVar2;
  byte *pbVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 *puVar6;
  undefined4 *unaff_EBP;
  int unaff_ESI;
  byte *local_478;
  uint uStack_46c;
  undefined4 uStack_20;
  
  DAT_000c40bc = -1;
  DAT_000c40bd = -1;
  DAT_000c40be = 0xff;
  puVar6 = (undefined4 *)((int)register0x00000010 + -0x1c);
  cVar2 = '\b';
  do {
    unaff_EBP = unaff_EBP + -1;
    puVar6 = puVar6 + -1;
    *puVar6 = *unaff_EBP;
    cVar2 = cVar2 + -1;
  } while ('\0' < cVar2);
  *(char *)(unaff_ESI + -0x77) = *(char *)(unaff_ESI + -0x77) + -1;
  pbVar3 = (byte *)FUN_00074c13();
  uVar4 = 0;
  if (pbVar3 != (byte *)0x0) {
    while( true ) {
      do {
        do {
          bVar1 = *pbVar3;
          pbVar3 = pbVar3 + 1;
        } while (bVar1 == 0x20);
      } while (bVar1 == 9);
      if (bVar1 == 0) break;
      pbVar3 = local_478;
      if (bVar1 < 0x49) {
        if (bVar1 < 0x44) {
          if (bVar1 == 0x41) goto LAB_000198bd;
          FUN_00075136();
        }
        else {
          if (bVar1 < 0x45) goto LAB_000198e8;
          if (bVar1 == 0x48) {
LAB_000198fd:
            DAT_000c40be = FUN_00075136();
          }
          else {
            FUN_00075136();
          }
        }
      }
      else if (bVar1 < 0x4a) {
LAB_000198d3:
        DAT_000c40bc = FUN_00075136();
      }
      else if (bVar1 < 100) {
        if (bVar1 == 0x61) {
LAB_000198bd:
          DAT_000c40ba = FUN_00075136();
        }
        else {
          FUN_00075136();
        }
      }
      else if (bVar1 < 0x65) {
LAB_000198e8:
        DAT_000c40bd = FUN_00075136();
      }
      else {
        if (0x67 < bVar1) {
          if (bVar1 < 0x69) goto LAB_000198fd;
          if (bVar1 == 0x69) goto LAB_000198d3;
        }
        FUN_00075136();
      }
    }
    if (((DAT_000c40ba == -1) || (DAT_000c40bc == -1)) || (DAT_000c40bd == -1)) {
      uVar4 = 0;
    }
    else {
      DAT_000c40c0 = DAT_000c40ba + 4;
      DAT_000c40c2 = DAT_000c40ba + 5;
      DAT_000c40c4 = DAT_000c40ba + 6;
      DAT_000c40c6 = DAT_000c40ba + 10;
      DAT_000c40c8 = DAT_000c40ba + 0xc;
      DAT_000c4086 = DAT_000c40ba + 0xe;
      iVar5 = FUN_00019790();
      uVar4 = 0;
      if (iVar5 != 0) {
        DAT_000c40b8 = FUN_00019740();
        if (DAT_000c40b8 == -1) {
          return (ulonglong)uStack_46c << 0x20;
        }
        uVar4 = 1;
      }
    }
  }
  return CONCAT44(uStack_46c,uVar4);
}

