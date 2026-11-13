
void __regparm3 FUN_00078d88(undefined4 *param_1)

{
  uint *puVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  int unaff_EBX;
  uint *puVar5;
  uint *puVar6;
  
  if (param_1 == (undefined4 *)0x0) {
    return;
  }
  puVar5 = param_1 + -1;
  if ((*puVar5 & 1) == 0) {
    return;
  }
  uVar4 = *puVar5 & 0xfffffffe;
  puVar6 = (uint *)((int)puVar5 + uVar4);
  if ((*puVar6 & 1) == 0) {
    if (puVar6 == *(uint **)(unaff_EBX + 0xc)) {
      *(uint **)(unaff_EBX + 0xc) = puVar5;
    }
    *puVar5 = uVar4 + *puVar6;
    uVar4 = puVar6[1];
    puVar6 = (uint *)puVar6[2];
    *(uint **)(uVar4 + 8) = puVar6;
    puVar6[1] = uVar4;
    *(int *)(unaff_EBX + 0x1c) = *(int *)(unaff_EBX + 0x1c) + -1;
  }
  else {
    *puVar5 = uVar4;
    puVar6 = *(uint **)(unaff_EBX + 0xc);
    if (puVar5 < puVar6) {
      if (((uint *)puVar6[1] < puVar5) || (puVar6 = *(uint **)(unaff_EBX + 0x28), puVar5 < puVar6))
      goto LAB_00078e46;
    }
    else {
      puVar6 = (uint *)puVar6[2];
      if ((puVar5 < puVar6) ||
         (puVar6 = (uint *)(unaff_EBX + 0x20), *(uint **)(unaff_EBX + 0x24) < puVar5))
      goto LAB_00078e46;
    }
    uVar4 = *(uint *)(unaff_EBX + 0x1c);
    uVar2 = *(uint *)(unaff_EBX + 0x18) / (uVar4 + 1);
    if (uVar2 < uVar4) {
      iVar3 = uVar2 * 2;
      if (*(int *)(unaff_EBX + 0x18) - uVar4 <= uVar4) {
        iVar3 = 0;
      }
      puVar6 = (uint *)((int)puVar5 + *puVar5);
      do {
        uVar4 = *puVar6;
        if ((uVar4 & 1) == 0) goto LAB_00078e46;
        if (uVar4 == 0xffffffff) break;
        puVar6 = (uint *)((int)puVar6 + (uVar4 & 0xfffffffe));
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
    }
    puVar6 = *(uint **)(unaff_EBX + 0xc);
    if (puVar5 < puVar6) {
      puVar6 = *(uint **)(unaff_EBX + 0x28);
    }
    while (((puVar6 <= puVar5 && (puVar6 = (uint *)puVar6[2], puVar6 <= puVar5)) &&
           (puVar6 = (uint *)puVar6[2], puVar6 <= puVar5))) {
      puVar6 = (uint *)puVar6[2];
    }
  }
LAB_00078e46:
  puVar1 = (uint *)puVar6[1];
  uVar4 = *puVar5;
  if ((uint *)((int)puVar1 + *puVar1) == puVar5) {
    uVar4 = uVar4 + *puVar1;
    *puVar1 = uVar4;
    if (puVar5 == *(uint **)(unaff_EBX + 0xc)) {
      *(uint **)(unaff_EBX + 0xc) = puVar1;
    }
    puVar5 = puVar1;
  }
  else {
    *(int *)(unaff_EBX + 0x1c) = *(int *)(unaff_EBX + 0x1c) + 1;
    param_1[1] = puVar6;
    *param_1 = puVar1;
    puVar1[2] = (uint)puVar5;
    puVar6[1] = (uint)puVar5;
  }
  *(int *)(unaff_EBX + 0x18) = *(int *)(unaff_EBX + 0x18) + -1;
  if ((puVar5 < *(uint **)(unaff_EBX + 0xc)) && (*(uint *)(unaff_EBX + 0x10) < uVar4)) {
    *(uint *)(unaff_EBX + 0x10) = uVar4;
  }
  if (*(uint *)(unaff_EBX + 0x14) < uVar4) {
    *(uint *)(unaff_EBX + 0x14) = uVar4;
  }
  return;
}

