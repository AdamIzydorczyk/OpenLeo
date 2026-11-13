
/* WARNING: Removing unreachable block (ram,0x0007f091) */

void __regparm3 FUN_0007f010(longlong *param_1)

{
  ushort uVar1;
  longlong *plVar2;
  char cVar3;
  uint uVar4;
  short sVar5;
  uint *puVar6;
  undefined1 *puVar7;
  ushort uVar8;
  uint unaff_ESI;
  uint uVar9;
  longlong lVar10;
  longlong lVar11;
  undefined4 uStack_30;
  longlong *local_2c;
  uint uVar12;
  uint uStack_24;
  undefined8 local_20;
  ushort local_18;
  
  uStack_24 = (uint)(ushort)((*(ushort *)(param_1 + 1) & 0x7fff) + 0xc002);
  lVar11 = param_1[1];
  uVar9 = CONCAT22((short)((uint)param_1 >> 0x10),0x3ffe);
  uStack_30 = 0x7f051;
  local_2c = param_1;
  lVar10 = FUN_0007ebb3();
  plVar2 = local_2c;
  *local_2c = lVar10;
  *(short *)(local_2c + 1) = (short)lVar11;
  puVar6 = &uStack_24;
  uVar4 = uVar9;
  uVar12 = unaff_ESI;
  if ((*(byte *)((int)local_2c + 9) & 0x80) != 0) {
    local_2c = (longlong *)0x3ffe;
    uStack_30 = 0xc90fdaa2;
    FUN_0007dee2();
    puVar6 = &uStack_30;
    uVar4 = 0x2168c234;
    uVar12 = uVar9;
  }
  puVar7 = (undefined1 *)((int)puVar6 + -4);
  *(uint *)((int)puVar6 + -4) =
       CONCAT31((int3)(CONCAT22((short)(uVar4 >> 0x10),CONCAT11((byte)unaff_ESI,(char)uVar4)) >> 8),
                (char)uVar4 + ((byte)unaff_ESI & 2)) & 0xffffff07;
  sVar5 = (short)plVar2[1] + -0x3ffe;
  cVar3 = (char)uVar12;
  if (sVar5 < -0x20) {
    local_20 = -0x8000000000000000;
    local_18 = 0x3fff;
  }
  else {
    if (-1 < sVar5) {
      sVar5 = 0;
    }
    uVar4 = (uint)(ushort)-sVar5;
    if (8 < -sVar5) {
      uVar4 = 8;
    }
    *(uint *)((int)puVar6 + -8) = (uint)*(ushort *)((int)&PTR_LAB_0007effe + uVar4 * 2);
    uVar4 = CONCAT31((int3)(uVar12 >> 8),cVar3 + 1U) & 0xffffff02;
    if (((cVar3 + 1U & 2) != 0) || ((char)(uVar4 >> 8) == '\x01')) {
      *(uint *)((int)puVar6 + -0xc) = uVar4;
      uVar8 = *(ushort *)(plVar2 + 1);
      *(undefined4 *)((int)puVar6 + -0x10) = 0x7f121;
      FUN_0007e316();
      *(undefined4 *)((int)puVar6 + -0x10) = 0x7f13a;
      local_20 = FUN_0007f514();
      uVar4 = *(uint *)((int)puVar6 + -0xc);
      local_18 = uVar8;
    }
    puVar7 = (undefined1 *)((int)puVar6 + -8);
    if (((char)uVar4 == '\0') || ((char)(uVar4 >> 8) == '\x01')) {
      *(undefined4 *)((int)puVar6 + -0xc) = 0x7f16e;
      FUN_0007f566();
      puVar7 = (undefined1 *)((int)puVar6 + -8);
    }
  }
  if ((cVar3 + 1U & 2) == 0) {
    lVar11 = *plVar2;
    uVar8 = *(ushort *)(plVar2 + 1);
  }
  else {
    uVar8 = local_18;
    lVar11 = local_20;
  }
  if ((uVar12 & 4) != 0) {
    uVar8 = uVar8 ^ 0x8000;
  }
  if ((char)(uVar12 >> 8) == '\x01') {
    if ((cVar3 + 3U & 2) == 0) {
      lVar11 = *plVar2;
      uVar1 = *(ushort *)(plVar2 + 1);
    }
    else {
      uVar1 = local_18;
      lVar11 = local_20;
    }
    if ((lVar11 == 0) && ((uVar1 & 0x7fff) == 0)) {
      lVar11 = -0x8000000000000000;
      uVar8 = uVar8 | 0x7fff;
    }
    else {
      *(undefined4 *)(puVar7 + -4) = 0x7f1f6;
      lVar11 = FUN_0007e141();
    }
  }
  *plVar2 = lVar11;
  *(ushort *)(plVar2 + 1) = uVar8;
  return;
}

