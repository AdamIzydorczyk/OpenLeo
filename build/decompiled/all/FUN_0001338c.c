
undefined8 FUN_0001338c(void)

{
  undefined2 uVar1;
  undefined4 uVar2;
  int *piVar3;
  uint *puVar4;
  int iVar5;
  byte *pbVar6;
  ushort auStack_24 [2];
  uint local_20 [2];
  ushort local_18 [2];
  
  puVar4 = local_20 + 1;
  local_20[1] = 0;
  do {
    if (DAT_000aee9d <= (ushort)*puVar4) {
      uVar2 = 1;
LAB_00013434:
      return CONCAT44(*(undefined4 *)((int)puVar4 + 0x10),uVar2);
    }
    *(ushort *)((int)puVar4 + 4) = 0;
    *(ushort *)((int)puVar4 + 6) = 0;
    pbVar6 = (byte *)(DAT_000aefce + (uint)*(ushort *)puVar4 * 0xd9);
    while ((uint)*(ushort *)(puVar4 + 1) < (uint)*pbVar6) {
      iVar5 = *(int *)(pbVar6 + 0xd5) + (uint)*(ushort *)(puVar4 + 1) * 0x1d;
      piVar3 = (int *)puVar4;
      if (*(int *)(iVar5 + 5) != 0) {
        if (*(int *)(iVar5 + 0x13) != 0) {
          puVar4[-1] = 0x13403;
          FUN_000169b4();
        }
        puVar4[-1] = (uint)*(ushort *)(iVar5 + 0x11);
        puVar4[-2] = 0x1341d;
        uVar1 = FUN_00012980();
        piVar3 = (int *)(puVar4 + -1);
        puVar4 = puVar4 + -1;
        *(undefined2 *)(iVar5 + 0x1b) = uVar1;
        if (*(int *)(iVar5 + 0x19) >> 0x10 < 0) {
          uVar2 = 0;
          goto LAB_00013434;
        }
      }
      *(int *)((int)piVar3 + 4) = *(int *)((int)piVar3 + 4) + 1;
      puVar4 = (uint *)piVar3;
    }
    *puVar4 = *puVar4 + 1;
  } while( true );
}

