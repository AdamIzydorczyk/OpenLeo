
undefined8 __regparm3 crt_malloc(uint param_1,undefined4 param_2)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  int extraout_ECX;
  uint uVar4;
  undefined8 uVar5;
  
  if ((param_1 == 0) || (0xffffffd4 < param_1)) {
    iVar2 = 0;
  }
  else {
    bVar1 = false;
    uVar4 = param_1 + 3 & 0xfffffffc;
    while( true ) {
      while( true ) {
        uVar3 = uVar4;
        if (uVar4 < 0xc) {
          uVar3 = 0xc;
        }
        if ((uVar3 <= DAT_000a9b60) || (iVar2 = DAT_000a9b5c, DAT_000a9b5c == 0)) {
          DAT_000a9b60 = 0;
          iVar2 = DAT_000a9b58;
        }
        while (iVar2 != 0) {
          DAT_000a9b5c = iVar2;
          iVar2 = FUN_00078ce0();
          if (iVar2 != 0) goto LAB_000752e9;
          if (DAT_000a9b60 < *(uint *)(extraout_ECX + 0x14)) {
            DAT_000a9b60 = *(uint *)(extraout_ECX + 0x14);
          }
          iVar2 = *(int *)(extraout_ECX + 8);
        }
        if ((bVar1) || (iVar2 = FUN_00079284(), iVar2 == 0)) break;
        bVar1 = true;
      }
      uVar5 = FUN_0007944d();
      iVar2 = (int)((ulonglong)uVar5 >> 0x20);
      if ((int)uVar5 == 0) break;
      bVar1 = false;
    }
LAB_000752e9:
    DAT_000cf9c0 = 0;
  }
  return CONCAT44(param_2,iVar2);
}

