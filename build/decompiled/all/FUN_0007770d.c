
undefined8 __regparm3 FUN_0007770d(uint param_1,undefined4 param_2,undefined4 param_3)

{
  uint extraout_ECX;
  undefined4 *extraout_EDX;
  undefined4 *puVar1;
  int iVar2;
  
  iVar2 = 0;
  for (puVar1 = DAT_000cfa34; puVar1 != (undefined4 *)0x0; puVar1 = (undefined4 *)*puVar1) {
    if (((*(uint *)(puVar1[1] + 0xc) & param_1) != 0) &&
       (iVar2 = iVar2 + 1, (*(byte *)(puVar1[1] + 0xd) & 0x10) != 0)) {
      FUN_0007a4e4(param_2,param_3);
      param_1 = extraout_ECX;
      puVar1 = extraout_EDX;
    }
  }
  return CONCAT44(param_2,iVar2);
}

