
undefined8 __regparm3 FUN_00079103(uint param_1,undefined4 param_2,undefined4 param_3)

{
  code *pcVar1;
  bool bVar2;
  undefined2 *puVar3;
  int iVar4;
  int *piVar5;
  undefined2 extraout_CX;
  undefined2 extraout_CX_00;
  undefined2 extraout_DX;
  int unaff_EBX;
  undefined4 *puVar6;
  uint unaff_ESI;
  undefined1 *unaff_EDI;
  char cVar7;
  byte bVar8;
  undefined1 auStack_94 [128];
  undefined1 auStack_a [2];
  undefined4 uStack_8;
  
  cVar7 = (param_1 >> 0xf & 1) != 0;
  uStack_8 = param_3;
  FUN_00078ee4();
  pcVar1 = (code *)swi(0x31);
  (*pcVar1)();
  puVar6 = (undefined4 *)(auStack_94 + 0x80);
  puVar3 = (undefined2 *)0x0;
  if (cVar7 != '\x01') {
    puVar3 = (undefined2 *)CONCAT22((short)(param_1 >> 0x10),extraout_CX);
    *puVar3 = (short)unaff_EDI;
    puVar3[1] = (short)unaff_ESI;
  }
  if (puVar3 == (undefined2 *)0x0) {
    if ((DAT_000cfa48 & 0xfff00000) != 0) {
      DAT_000cfa48 = 0xfffff;
    }
    puVar6 = (undefined4 *)(auStack_94 + 0x80);
    if (unaff_EDI < &LAB_00010001) {
      bVar8 = 0;
      pcVar1 = (code *)swi(0x21);
      (*pcVar1)();
      puVar6 = (undefined4 *)(auStack_94 + 0x84);
      if ((bVar8 & 1) == 0) {
        bVar8 = (unaff_ESI >> 3 & 1) != 0;
        pcVar1 = (code *)swi(0x21);
        iVar4 = (*pcVar1)();
        bVar2 = (bVar8 & 1) != 0;
        pcVar1 = (code *)swi(0x21);
        (*pcVar1)();
        puVar6 = &uStack_8;
        if (!bVar2) {
          pcVar1 = (code *)swi(0x31);
          (*pcVar1)();
          puVar6 = (undefined4 *)(auStack_a + 6);
          piVar5 = (int *)(CONCAT22(extraout_CX_00,extraout_DX) + 8);
          *(uint *)(CONCAT22(extraout_CX_00,extraout_DX) + 4) =
               (iVar4 << 1 | (uint)bVar8) >> 1 | (uint)bVar2 << 0x1f;
          *piVar5 = unaff_EBX + -8;
          goto LAB_000791cb;
        }
      }
    }
    piVar5 = (int *)0x0;
  }
  else {
    piVar5 = (int *)(puVar3 + 4);
    *piVar5 = (int)(unaff_EDI + -8);
    *(undefined4 *)(puVar3 + 2) = 0;
  }
LAB_000791cb:
  return CONCAT44(*(undefined4 *)((int)puVar6 + 0xc),piVar5);
}

