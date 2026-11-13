
undefined8 __regparm3 FUN_000754b9(int *param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  int *piVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  undefined4 *puVar4;
  
  puVar4 = (undefined4 *)*param_1;
  iVar1 = FUN_00077ca4();
  uVar2 = 0;
  if (iVar1 != 0) {
    if ((puVar4 != *(undefined4 **)(extraout_ECX + 0x14)) ||
       (*(int *)(iVar1 + 4) + 2 != *(int *)(extraout_ECX + 0x18))) {
      for (; puVar4 != (undefined4 *)0x0; puVar4 = (undefined4 *)*puVar4) {
        if (puVar4 == *(undefined4 **)(extraout_ECX + 0x14)) {
          FUN_00077cd2();
          goto LAB_0007550b;
        }
      }
    }
LAB_000754e7:
    uVar2 = 1;
  }
LAB_00075530:
  return CONCAT44(param_2,uVar2);
LAB_0007550b:
  piVar3 = (int *)FUN_00077d7d();
  if (piVar3 == (int *)0x0) goto LAB_000754e7;
  if (((*piVar3 == 0) && (*(short *)piVar3[1] == 3)) &&
     ((short *)piVar3[1] + 1 == *(short **)(extraout_ECX_00 + 0x18))) {
    uVar2 = 0;
    goto LAB_00075530;
  }
  FUN_00077cf8();
  goto LAB_0007550b;
}

