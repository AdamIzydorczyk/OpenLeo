
undefined1 __regparm3 FUN_00075773(int param_1)

{
  byte bVar1;
  ushort uVar2;
  int iVar3;
  int *piVar4;
  int extraout_ECX;
  int extraout_ECX_00;
  undefined1 extraout_DL;
  undefined1 uVar5;
  int extraout_EDX;
  undefined4 *extraout_EDX_00;
  undefined4 *puVar6;
  undefined4 local_18;
  
  if ((*(byte *)(param_1 + 0x24) & 2) != 0) {
    iVar3 = *(int *)(param_1 + 0xc);
    if (iVar3 == 0) {
      FUN_0007539b();
      iVar3 = extraout_EDX;
    }
    *(undefined4 *)(param_1 + 4) = *(undefined4 *)(iVar3 + 4);
    bVar1 = *(byte *)(iVar3 + 0x1d);
    *(byte *)(param_1 + 0x24) = *(byte *)(param_1 + 0x24) & 0xfe;
    *(uint *)(param_1 + 0x24) = *(uint *)(param_1 + 0x24) | (uint)(bVar1 & 1);
  }
  FUN_00077cd2();
  do {
    piVar4 = (int *)FUN_00077d7d();
    if (piVar4 == (int *)0x0) {
      return 0;
    }
    if (*piVar4 == 0) {
      uVar2 = *(ushort *)piVar4[1];
      if (uVar2 < 3) {
        if ((uVar2 == 2) && (iVar3 = FUN_00075485(), iVar3 != 0)) {
          uVar5 = 1;
LAB_00075848:
          *(undefined4 *)(param_1 + 0x1c) = local_18;
          return uVar5;
        }
      }
      else if (uVar2 < 4) {
        puVar6 = *(undefined4 **)(param_1 + 0xc);
        iVar3 = extraout_ECX_00;
        if (puVar6 == (undefined4 *)0x0) {
          FUN_0007539b();
          iVar3 = extraout_ECX;
          puVar6 = extraout_EDX_00;
        }
        if (iVar3 == 0) {
          *(undefined4 *)(param_1 + 0xc) = *puVar6;
        }
      }
      else if ((uVar2 == 4) && (iVar3 = FUN_00075448(), iVar3 != 0)) {
        FUN_00077cf8();
        FUN_00077d7d();
        uVar5 = extraout_DL;
        goto LAB_00075848;
      }
    }
    FUN_00077cf8();
  } while( true );
}

