
undefined4 __regparm3 FUN_0007fca6(undefined4 param_1,uint param_2,uint *param_3)

{
  uint *puVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint unaff_EBX;
  uint *puVar6;
  undefined4 uVar7;
  uint unaff_ESI;
  short in_DS;
  
  uVar3 = unaff_EBX + 7 & 0xfffffffc;
  if (uVar3 < unaff_EBX) {
    uVar3 = 0xffffffff;
  }
  if (uVar3 < 0xc) {
    uVar3 = 0xc;
  }
  puVar1 = (uint *)(param_2 - 4);
  uVar5 = *puVar1 & 0xfffffffe;
  if (uVar5 < uVar3) {
    uVar3 = uVar3 - uVar5;
    puVar6 = (uint *)((int)puVar1 + uVar5);
    while( true ) {
      *param_3 = uVar3;
      uVar5 = *puVar6;
      if (uVar5 == 0xffffffff) break;
      if ((uVar5 & 1) != 0) {
        return 1;
      }
      uVar3 = puVar6[2];
      uVar2 = puVar6[1];
      if (in_DS == (short)param_1) {
        for (unaff_ESI = DAT_000a9b58;
            (*(uint *)(unaff_ESI + 8) != 0 &&
            ((param_2 < unaff_ESI || (*(uint *)(unaff_ESI + 8) <= param_2))));
            unaff_ESI = *(uint *)(unaff_ESI + 8)) {
        }
      }
      if (puVar6 == *(uint **)(unaff_ESI + 0xc)) {
        *(uint *)(unaff_ESI + 0xc) = (*(uint **)(unaff_ESI + 0xc))[1];
      }
      if (*param_3 <= uVar5) {
        uVar4 = uVar5 - *param_3;
        if (0xb < uVar4) {
          puVar6 = (uint *)((int)puVar6 + *param_3);
          *puVar6 = uVar4;
          puVar6[1] = uVar2;
          puVar6[2] = uVar3;
          *(uint **)(uVar2 + 8) = puVar6;
          *(uint **)(uVar3 + 4) = puVar6;
          *puVar1 = *puVar1 + *param_3;
          DAT_000cf9c1 = 0;
          return 0;
        }
      }
      *(uint *)(uVar2 + 8) = uVar3;
      *(uint *)(uVar3 + 4) = uVar2;
      *puVar1 = *puVar1 + uVar5;
      *(int *)(unaff_ESI + 0x1c) = *(int *)(unaff_ESI + 0x1c) + -1;
      DAT_000cf9c1 = 0;
      if (*param_3 <= uVar5) goto LAB_0007fe2a;
      uVar3 = *param_3 - uVar5;
      puVar6 = (uint *)((int)puVar6 + uVar5);
    }
    uVar7 = 2;
  }
  else {
    if (0xb < uVar5 - uVar3) {
      *puVar1 = uVar3 | 1;
      *(uint *)((int)puVar1 + uVar3) = uVar5 - uVar3 | 1;
      if (in_DS == (short)param_1) {
        for (unaff_ESI = DAT_000a9b58;
            (*(uint *)(unaff_ESI + 8) != 0 &&
            ((param_2 < unaff_ESI || (*(uint *)(unaff_ESI + 8) <= param_2))));
            unaff_ESI = *(uint *)(unaff_ESI + 8)) {
        }
      }
      *(int *)(unaff_ESI + 0x18) = *(int *)(unaff_ESI + 0x18) + 1;
      FUN_00075302(param_1,(uint *)((int)puVar1 + uVar3));
    }
LAB_0007fe2a:
    uVar7 = 0;
  }
  return uVar7;
}

