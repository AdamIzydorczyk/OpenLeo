
undefined8 __regparm3 FUN_00077d7d(int param_1,undefined4 param_2)

{
  ushort uVar1;
  ushort *puVar2;
  char cVar3;
  int *piVar4;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar5;
  undefined4 uVar6;
  
  uVar6 = *(undefined4 *)(param_1 + 4);
  do {
    while( true ) {
      piVar4 = (int *)FUN_00077ca4();
      iVar5 = extraout_ECX;
      if (((piVar4 == (int *)0x0) || (*(int *)(extraout_ECX + 4) == *(int *)(extraout_ECX + 9))) ||
         (*piVar4 != 0)) goto LAB_00077e04;
      puVar2 = (ushort *)piVar4[1];
      uVar1 = *puVar2;
      if (1 < uVar1) break;
      if (uVar1 != 1) {
LAB_00077df5:
        FUN_0007539b();
        iVar5 = extraout_ECX_00;
LAB_00077e04:
        *(undefined4 *)(iVar5 + 4) = uVar6;
        return CONCAT44(param_2,piVar4);
      }
      uVar6 = *(undefined4 *)(puVar2 + 1);
    }
    if (uVar1 < 5) goto LAB_00077e04;
    if (5 < uVar1) {
      if (0xb < uVar1) goto LAB_00077df5;
      goto LAB_00077e04;
    }
    cVar3 = FUN_0007be5c();
    if (cVar3 == '\0') {
      uVar6 = *(undefined4 *)(puVar2 + 5);
    }
    else {
      uVar6 = *(undefined4 *)(puVar2 + 3);
    }
  } while( true );
}

