
void __regparm3 FUN_00054030(undefined4 param_1,int param_2,undefined4 param_3)

{
  undefined1 uVar1;
  uint uVar2;
  int iVar3;
  
  iVar3 = FUN_000532c0(param_3);
  *(undefined4 *)(iVar3 + 0xda) = 0;
  *(undefined ***)(iVar3 + 0xde) = &PTR_LAB_00098278;
  *(undefined4 *)(iVar3 + 0xd6) = 0;
  *(undefined ***)(iVar3 + 8) = &PTR_LAB_000a23b8;
  *(undefined ***)(iVar3 + 0xc) = &PTR_LAB_000a23b0;
  *(undefined4 *)(iVar3 + 0xa5) = *(undefined4 *)(param_2 + 0xa5);
  *(undefined4 *)(iVar3 + 0xa9) = *(undefined4 *)(param_2 + 0xa9);
  *(undefined4 *)(iVar3 + 0xad) = *(undefined4 *)(param_2 + 0xad);
  *(undefined4 *)(iVar3 + 0xb1) = *(undefined4 *)(param_2 + 0xb1);
  uVar2 = *(uint *)(param_2 + 0xb6);
  *(byte *)(iVar3 + 0xb6) = *(byte *)(iVar3 + 0xb6) & 0xfe;
  *(uint *)(iVar3 + 0xb6) = *(uint *)(iVar3 + 0xb6) | uVar2 & 1;
  uVar2 = *(uint *)(param_2 + 0xb6);
  *(byte *)(iVar3 + 0xb6) = *(byte *)(iVar3 + 0xb6) & 0xfd;
  *(uint *)(iVar3 + 0xb6) = *(uint *)(iVar3 + 0xb6) | uVar2 & 2;
  uVar2 = *(uint *)(param_2 + 0xb6);
  *(byte *)(iVar3 + 0xb6) = *(byte *)(iVar3 + 0xb6) & 0xfb;
  *(uint *)(iVar3 + 0xb6) = *(uint *)(iVar3 + 0xb6) | uVar2 & 4;
  uVar2 = *(uint *)(param_2 + 0xb6);
  *(byte *)(iVar3 + 0xb6) = *(byte *)(iVar3 + 0xb6) & 0xf7;
  *(uint *)(iVar3 + 0xb6) = *(uint *)(iVar3 + 0xb6) | uVar2 & 8;
  uVar2 = *(uint *)(param_2 + 0xb6);
  *(byte *)(iVar3 + 0xb6) = *(byte *)(iVar3 + 0xb6) & 0xef;
  *(uint *)(iVar3 + 0xb6) = *(uint *)(iVar3 + 0xb6) | uVar2 & 0x10;
  uVar2 = *(uint *)(param_2 + 0xb6);
  *(byte *)(iVar3 + 0xb6) = *(byte *)(iVar3 + 0xb6) & 0xdf;
  *(uint *)(iVar3 + 0xb6) = *(uint *)(iVar3 + 0xb6) | uVar2 & 0x20;
  *(undefined4 *)(iVar3 + 0xba) = *(undefined4 *)(param_2 + 0xba);
  *(undefined4 *)(iVar3 + 0xbe) = *(undefined4 *)(param_2 + 0xbe);
  *(undefined4 *)(iVar3 + 0xc2) = *(undefined4 *)(param_2 + 0xc2);
  *(undefined4 *)(iVar3 + 0xc6) = *(undefined4 *)(param_2 + 0xc6);
  *(undefined4 *)(iVar3 + 0xca) = *(undefined4 *)(param_2 + 0xca);
  *(undefined4 *)(iVar3 + 0xce) = *(undefined4 *)(param_2 + 0xce);
  *(undefined4 *)(iVar3 + 0xe6) = *(undefined4 *)(param_2 + 0xe6);
  *(undefined4 *)(iVar3 + 0xe2) = *(undefined4 *)(param_2 + 0xe2);
  uVar1 = *(undefined1 *)(param_2 + 0xb5);
  *(undefined4 *)(iVar3 + 0xd2) = 0;
  *(undefined1 *)(iVar3 + 0xb5) = uVar1;
  return;
}

