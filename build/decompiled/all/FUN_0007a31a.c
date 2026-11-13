
undefined8 __regparm3 FUN_0007a31a(undefined4 param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  int iVar2;
  undefined *puVar3;
  int extraout_ECX;
  uint uVar4;
  
  if (DAT_000cf9bc == (undefined4 *)0x0) {
    for (puVar3 = &DAT_000a9c00; puVar3 < &DAT_000a9e08; puVar3 = puVar3 + 0x1a) {
      if ((puVar3[0xc] & 3) == 0) {
        puVar1 = (undefined4 *)crt_malloc();
        if (puVar1 == (undefined4 *)0x0) goto LAB_0007a3ae;
        uVar4 = 3;
        goto LAB_0007a389;
      }
    }
    uVar4 = 0x4003;
    puVar1 = (undefined4 *)crt_malloc();
    if (puVar1 == (undefined4 *)0x0) {
LAB_0007a3ae:
      FUN_00078c12();
      iVar2 = 0;
      goto LAB_0007a3ba;
    }
  }
  else {
    uVar4 = (uint)((ushort)*(undefined4 *)(DAT_000cf9bc[1] + 0xc) & 0x4003 | 3);
    puVar1 = DAT_000cf9bc;
    DAT_000cf9bc = (undefined4 *)*DAT_000cf9bc;
  }
LAB_0007a389:
  FUN_000749a0();
  *(uint *)(extraout_ECX + 0xc) = uVar4;
  puVar1[1] = extraout_ECX;
  *puVar1 = DAT_000cfa34;
  DAT_000cfa34 = puVar1;
  iVar2 = extraout_ECX;
LAB_0007a3ba:
  return CONCAT44(param_2,iVar2);
}

