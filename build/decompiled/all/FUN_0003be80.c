
void __regparm3 FUN_0003be80(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined8 uVar4;
  
  uVar4 = FUN_0003bcf0(param_2,param_3);
  iVar3 = (int)((ulonglong)uVar4 >> 0x20);
  if ((int)uVar4 == 0) {
    iVar1 = *(int *)(iVar3 + 0xee) + -1;
    *(int *)(iVar3 + 0xf2) = iVar1;
    *(int *)(iVar3 + 0xf6) = *(int *)(iVar3 + 0x10e + iVar1 * 0xb8) + -2;
  }
  else {
    *(undefined4 *)(iVar3 + 0xf6) = 1;
    *(undefined4 *)(iVar3 + 0xf2) = *(undefined4 *)(iVar3 + 0xee);
  }
  uVar4 = FUN_0003bd10();
  iVar3 = (int)((ulonglong)uVar4 >> 0x20);
  if ((int)uVar4 == 0) {
    puVar2 = (undefined4 *)
             (iVar3 + *(int *)(iVar3 + 0xf2) * 0xb8 + 0x112 + *(int *)(iVar3 + 0xf6) * 0xc);
    *(undefined4 *)(iVar3 + 0xfa) = *puVar2;
    *(undefined4 *)(iVar3 + 0xfe) = puVar2[1];
  }
  return;
}

