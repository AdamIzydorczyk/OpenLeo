
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

short FUN_0001aeda(void)

{
  code *pcVar1;
  ushort uVar2;
  short sVar3;
  ushort uVar4;
  uint uVar5;
  undefined2 extraout_var;
  uint uVar6;
  undefined2 *unaff_ESI;
  undefined2 *unaff_EDI;
  byte bVar7;
  ushort unaff_retaddr;
  
  bVar7 = 0;
  uVar4 = (ushort)DAT_000c41c4;
  uVar2 = (ushort)unaff_EDI + uVar4;
  uVar5 = DAT_000c41c4;
  if (CARRY2((ushort)unaff_EDI,uVar4)) {
    uVar4 = uVar4 - uVar2;
    for (uVar5 = CONCAT22((short)(DAT_000c41c4 >> 0x10),uVar4) >> 1; uVar5 != 0; uVar5 = uVar5 - 1)
    {
      *unaff_EDI = *unaff_ESI;
      unaff_ESI = unaff_ESI + 1;
      unaff_EDI = unaff_EDI + 1;
    }
    for (uVar5 = (uint)((uVar4 & 1) != 0); uVar5 != 0; uVar5 = uVar5 - 1) {
      *(undefined1 *)unaff_EDI = *(undefined1 *)unaff_ESI;
      unaff_ESI = (undefined2 *)((int)unaff_ESI + 1);
      unaff_EDI = (undefined2 *)((int)unaff_EDI + 1);
    }
    DAT_000a8974 = DAT_000a8974 + 1;
    pcVar1 = (code *)swi(0x10);
    (*pcVar1)();
    uVar5 = CONCAT22(extraout_var,unaff_retaddr);
    unaff_EDI = DAT_000c41bc;
    uVar2 = unaff_retaddr;
  }
  for (uVar6 = uVar5 >> 1; uVar6 != 0; uVar6 = uVar6 - 1) {
    *unaff_EDI = *unaff_ESI;
    unaff_ESI = unaff_ESI + (uint)bVar7 * -2 + 1;
    unaff_EDI = unaff_EDI + (uint)bVar7 * -2 + 1;
  }
  for (uVar5 = (uint)((uVar5 & 1) != 0); uVar5 != 0; uVar5 = uVar5 - 1) {
    *(undefined1 *)unaff_EDI = *(undefined1 *)unaff_ESI;
    unaff_ESI = (undefined2 *)((int)unaff_ESI + (uint)bVar7 * -2 + 1);
    unaff_EDI = (undefined2 *)((int)unaff_EDI + (uint)bVar7 * -2 + 1);
  }
  sVar3 = uVar2 + _DAT_000a8960;
  if (CARRY2(uVar2,_DAT_000a8960)) {
    DAT_000a8974 = DAT_000a8974 + 1;
    pcVar1 = (code *)swi(0x10);
    sVar3 = (*pcVar1)();
  }
  return sVar3;
}

