
undefined4 * __regparm3 FUN_0007754a(char *param_1,undefined1 param_2)

{
  char cVar1;
  code *pcVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined1 *puVar6;
  undefined1 *puVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  byte bVar10;
  byte bVar11;
  undefined4 uStack_e0;
  undefined1 auStack_dc [4];
  undefined1 local_d8 [169];
  byte local_2f;
  undefined1 local_18;
  
  bVar11 = 0;
  pcVar2 = (code *)swi(0x21);
  local_18 = param_2;
  (*pcVar2)();
  iVar3 = FUN_0007751d();
  puVar7 = auStack_dc;
  if (iVar3 == 0) {
    bVar10 = 0;
    pcVar2 = (code *)swi(0x21);
    (*pcVar2)();
    puVar6 = local_d8;
    puVar7 = local_d8;
    if ((bVar10 & 1) != 0) {
LAB_00077594:
      *(undefined4 *)(puVar6 + -4) = 0x7759e;
      FUN_0007ba5f();
      return (undefined4 *)0x0;
    }
  }
  else {
    local_2f = 0x10;
  }
  if ((local_2f & 0x10) != 0) {
    iVar3 = 0;
    while (cVar1 = *param_1, puVar7[iVar3] = cVar1, cVar1 != '\0') {
      if ((*param_1 == '*') || (*param_1 == '?')) goto LAB_0007761d;
      iVar3 = iVar3 + 1;
      param_1 = param_1 + 1;
      if (0x8f < iVar3) goto LAB_0007761d;
    }
    iVar4 = iVar3;
    if (((iVar3 != 0) && (puVar7[iVar3 + -1] != '\\')) && (puVar7[iVar3 + -1] != '/')) {
      iVar4 = iVar3 + 1;
      puVar7[iVar3] = 0x5c;
    }
    bVar10 = 0;
    *(undefined4 *)(puVar7 + iVar4) = DAT_00095ec0;
    pcVar2 = (code *)swi(0x21);
    (*pcVar2)();
    puVar6 = puVar7 + 4;
    puVar7 = puVar7 + 4;
    if ((bVar10 & 1) != 0) goto LAB_00077594;
  }
LAB_0007761d:
  *(undefined4 *)(puVar7 + -4) = 0x77627;
  puVar5 = (undefined4 *)crt_malloc();
  if (puVar5 != (undefined4 *)0x0) {
    puVar8 = (undefined4 *)(puVar7 + 0x98);
    puVar9 = puVar5;
    for (iVar3 = 0xb; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar9 = *puVar8;
      puVar8 = puVar8 + (uint)bVar11 * -2 + 1;
      puVar9 = puVar9 + (uint)bVar11 * -2 + 1;
    }
    *(undefined2 *)puVar9 = *(undefined2 *)puVar8;
    *(undefined1 *)((int)puVar5 + 0x2d) = 1;
  }
  return puVar5;
}

