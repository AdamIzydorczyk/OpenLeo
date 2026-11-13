
void __regparm3 FUN_0003bde0(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 *puVar1;
  int iVar2;
  undefined8 uVar3;
  
  uVar3 = FUN_0003bcf0(param_2,param_3);
  iVar2 = (int)((ulonglong)uVar3 >> 0x20);
  if ((int)uVar3 == 0) {
    *(int *)(iVar2 + 0xf6) = *(int *)(iVar2 + 0xf6) + -1;
  }
  else {
    *(int *)(iVar2 + 0xf6) = *(int *)(iVar2 + 0xf6) + 1;
  }
  puVar1 = (undefined4 *)
           (iVar2 + *(int *)(iVar2 + 0xf2) * 0xb8 + 0x112 + *(int *)(iVar2 + 0xf6) * 0xc);
  *(undefined4 *)(iVar2 + 0xfa) = *puVar1;
  *(undefined4 *)(iVar2 + 0xfe) = puVar1[1];
  return;
}

