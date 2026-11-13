
undefined4 * __regparm3 FUN_0007baf1(undefined4 *param_1,int param_2)

{
  undefined4 *puVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  undefined4 *puVar5;
  byte bVar6;
  
  bVar6 = 0;
  if (param_1 == (undefined4 *)0x0) {
    puVar1 = (undefined4 *)crt_malloc();
  }
  else if (param_2 == 0) {
    FUN_00075302();
    puVar1 = (undefined4 *)0x0;
  }
  else {
    uVar2 = FUN_0007fc65();
    puVar1 = (undefined4 *)FUN_0007fc6e();
    if (puVar1 == (undefined4 *)0x0) {
      if ((DAT_000a9bee == '\x01') && (iVar3 = FUN_00078f40(), iVar3 != 0)) {
        return (undefined4 *)(iVar3 + 4);
      }
      puVar1 = (undefined4 *)crt_malloc();
      if (puVar1 == (undefined4 *)0x0) {
        FUN_0007fc6e();
      }
      else {
        puVar5 = puVar1;
        for (uVar4 = uVar2 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
          *puVar5 = *param_1;
          param_1 = param_1 + (uint)bVar6 * -2 + 1;
          puVar5 = puVar5 + (uint)bVar6 * -2 + 1;
        }
        for (uVar2 = uVar2 & 3; uVar2 != 0; uVar2 = uVar2 - 1) {
          *(undefined1 *)puVar5 = *(undefined1 *)param_1;
          param_1 = (undefined4 *)((int)param_1 + (uint)bVar6 * -2 + 1);
          puVar5 = (undefined4 *)((int)puVar5 + (uint)bVar6 * -2 + 1);
        }
        FUN_00075302();
      }
    }
  }
  return puVar1;
}

