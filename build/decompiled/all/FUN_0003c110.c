
undefined4 __regparm3 FUN_0003c110(int *param_1,undefined4 param_2)

{
  code *pcVar1;
  int iVar2;
  uint uVar3;
  int *unaff_EBX;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  byte bVar7;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined1 auStack_20 [16];
  undefined4 local_10;
  
  bVar7 = 0;
  uStack_24 = 0x3c125;
  local_10 = param_2;
  FUN_000552e0();
  puVar4 = (undefined4 *)auStack_20;
  if (*(int *)((int)param_1 + 0xf6) != 0) {
    uStack_24 = 0x3c13a;
    iVar2 = (**(code **)(unaff_EBX[2] + 0x3c))();
    puVar4 = (undefined4 *)auStack_20;
    if ((((iVar2 != 0) && (puVar4 = (undefined4 *)auStack_20, *(int *)((int)param_1 + 0xf2) != 0))
        && (uVar3 = *param_1 - *unaff_EBX >> 0x1f, puVar4 = (undefined4 *)auStack_20,
           (int)((*param_1 - *unaff_EBX ^ uVar3) - uVar3) < 0x1e)) &&
       (((uVar3 = param_1[1] - unaff_EBX[1] >> 0x1f, puVar4 = (undefined4 *)auStack_20,
         (int)((param_1[1] - unaff_EBX[1] ^ uVar3) - uVar3) < 100 &&
         (puVar4 = (undefined4 *)auStack_20, unaff_EBX[0x17] == 0x1b)) &&
        (puVar4 = (undefined4 *)auStack_20, 5 < *(int *)(*(int *)(unaff_EBX[0x14] + 0x6c) + 4))))) {
      uStack_24 = 0;
      *(byte *)((int)param_1 + 0x66) = *(byte *)((int)param_1 + 0x66) | 0x40;
      uStack_28 = 0x3c1ad;
      FUN_0004bc40();
      uStack_28 = 0;
      FUN_0004bc40();
      puVar4 = &uStack_28;
    }
    pcVar1 = *(code **)(unaff_EBX[2] + 0xc);
    *(undefined4 *)((int)puVar4 + -4) = 0x3c1d2;
    iVar2 = (*pcVar1)();
    if (iVar2 == DAT_000cee54) {
      *(undefined4 *)((int)param_1 + 0xf2) = 1;
    }
  }
  puVar6 = (undefined4 *)puVar4[4];
  *(undefined1 *)(puVar6 + 3) = *(undefined1 *)(puVar4 + 3);
  puVar4[-1] = puVar6;
  puVar5 = puVar4;
  for (iVar2 = 3; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar6 = *puVar5;
    puVar5 = puVar5 + (uint)bVar7 * -2 + 1;
    puVar6 = puVar6 + (uint)bVar7 * -2 + 1;
  }
  for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
    *(undefined1 *)puVar6 = *(undefined1 *)puVar5;
    puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar7 * -2 + 1);
    puVar6 = (undefined4 *)((int)puVar6 + (uint)bVar7 * -2 + 1);
  }
  return puVar4[4];
}

