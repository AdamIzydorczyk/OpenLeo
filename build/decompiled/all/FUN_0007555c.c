
void __regparm3 FUN_0007555c(int *param_1)

{
  byte bVar1;
  int iVar2;
  undefined1 *puVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  int *piVar8;
  int *piVar9;
  
  iVar5 = *param_1;
  iVar6 = param_1[3];
  iVar2 = param_1[6];
  *(undefined1 *)(iVar6 + 0x1c) = 1;
  puVar3 = *(undefined1 **)(iVar2 + 0xe + param_1[8] * 4);
  if (puVar3 == (undefined1 *)0x0) goto LAB_00075590;
  piVar8 = (int *)(iVar5 + *(int *)(iVar2 + 6));
  iVar4 = *(int *)(param_1[5] + 4);
  piVar9 = (int *)(iVar6 + 0x1e + iVar4);
  switch(*puVar3) {
  case 0:
    iVar5 = FUN_000754ab();
    (**(code **)(iVar5 + 5))();
    break;
  case 1:
    if ((*(byte *)(param_1 + 9) & 1) != 0) {
      *piVar8 = 0;
      break;
    }
    iVar6 = *(int *)(iVar6 + 0x1e) + iVar4;
LAB_000755af:
    *piVar8 = iVar6;
    break;
  case 2:
    bVar1 = **(byte **)(puVar3 + 1);
    if (bVar1 < 3) {
      if (bVar1 != 0) {
        if (bVar1 != 1) break;
        if ((*(byte *)(param_1 + 9) & 1) == 0) {
          *(int *)(iVar6 + 0xc) = *(int *)(iVar6 + 0x1e) + iVar4;
        }
        else {
          *(undefined4 *)(iVar6 + 0xc) = 0;
        }
        iVar6 = iVar6 + 0xc;
        goto LAB_000755af;
      }
    }
    else if ((3 < bVar1) && (4 < bVar1)) {
      if ((bVar1 < 6) || (8 < bVar1)) break;
      if ((*(byte *)(param_1 + 9) & 1) != 0) {
        piVar9 = (int *)(iVar6 + 0xc);
        *(undefined4 *)(iVar6 + 0xc) = 0;
      }
    }
    *piVar8 = (int)piVar9;
    break;
  case 4:
    iVar5 = FUN_000754ab();
    (**(code **)(iVar5 + 5))();
    break;
  default:
    FUN_0007539b();
    break;
  case 6:
  case 7:
  case 8:
    if ((*(byte *)(param_1 + 9) & 1) != 0) {
      FUN_000749a0(iVar5 + *(int *)(iVar2 + 2));
      break;
    }
  case 3:
    bVar1 = puVar3[1];
    for (uVar7 = (uint)(bVar1 >> 2); uVar7 != 0; uVar7 = uVar7 - 1) {
      *piVar8 = *piVar9;
      piVar9 = piVar9 + 1;
      piVar8 = piVar8 + 1;
    }
    for (uVar7 = bVar1 & 0xffffff03; uVar7 != 0; uVar7 = uVar7 - 1) {
      *(char *)piVar8 = (char)*piVar9;
      piVar9 = (int *)((int)piVar9 + 1);
      piVar8 = (int *)((int)piVar8 + 1);
    }
  }
LAB_00075590:
  FUN_00076e0f();
  return;
}

