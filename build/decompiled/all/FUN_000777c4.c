
undefined8 __regparm3 FUN_000777c4(char *param_1,undefined4 param_2)

{
  char cVar1;
  undefined4 uVar2;
  char *pcVar3;
  undefined4 *puVar4;
  int iVar5;
  uint extraout_ECX;
  uint uVar6;
  undefined4 *extraout_ECX_00;
  undefined4 *puVar7;
  undefined4 *puVar8;
  byte bVar9;
  int local_1c;
  
  puVar4 = DAT_000a9bb4;
  bVar9 = 0;
  pcVar3 = param_1;
  if (param_1 != (char *)0x0) {
    for (; (*pcVar3 != '\0' && (*pcVar3 != '=')); pcVar3 = pcVar3 + 1) {
    }
    if (*pcVar3 != '\0') {
      cVar1 = pcVar3[1];
      if (DAT_000a9bb4 == (undefined4 *)0x0) {
        if (cVar1 != '\0') {
          puVar4 = (undefined4 *)crt_malloc();
          if (puVar4 == (undefined4 *)0x0) goto LAB_000777d2;
          DAT_000a9bb4 = puVar4;
          *puVar4 = 0;
          puVar4[1] = 0;
          DAT_000a9bb8 = puVar4 + 2;
          local_1c = 0;
LAB_00077907:
          puVar4[local_1c] = param_1;
          *(undefined1 *)((int)DAT_000a9bb8 + local_1c) = 0;
        }
      }
      else {
        iVar5 = FUN_00077930();
        if (cVar1 != '\0') {
          if (0 < iVar5) {
            local_1c = iVar5 + -1;
            goto LAB_00077907;
          }
          local_1c = -iVar5;
          if (DAT_000a9bb8 == (undefined4 *)0x0) {
            puVar4 = (undefined4 *)crt_malloc();
            if (puVar4 != (undefined4 *)0x0) {
              puVar7 = DAT_000a9bb4;
              puVar8 = puVar4;
              for (uVar6 = extraout_ECX >> 2; uVar6 != 0; uVar6 = uVar6 - 1) {
                *puVar8 = *puVar7;
                puVar7 = puVar7 + (uint)bVar9 * -2 + 1;
                puVar8 = puVar8 + (uint)bVar9 * -2 + 1;
              }
              for (uVar6 = extraout_ECX & 3; uVar6 != 0; uVar6 = uVar6 - 1) {
                *(undefined1 *)puVar8 = *(undefined1 *)puVar7;
                puVar7 = (undefined4 *)((int)puVar7 + (uint)bVar9 * -2 + 1);
                puVar8 = (undefined4 *)((int)puVar8 + (uint)bVar9 * -2 + 1);
              }
              DAT_000a9bb8 = puVar4 + (2 - iVar5);
              FUN_000749a0();
LAB_000778ee:
              DAT_000a9bb4 = puVar4;
              puVar4[1 - iVar5] = 0;
              goto LAB_00077907;
            }
          }
          else {
            puVar4 = (undefined4 *)FUN_0007baf1();
            if (puVar4 != (undefined4 *)0x0) {
              FUN_0007bb8f();
              DAT_000a9bb8 = extraout_ECX_00;
              goto LAB_000778ee;
            }
          }
          goto LAB_000777d2;
        }
      }
      uVar2 = 0;
      goto LAB_00077926;
    }
  }
LAB_000777d2:
  uVar2 = 0xffffffff;
LAB_00077926:
  return CONCAT44(param_2,uVar2);
}

