
void __regparm3 FUN_00065090(int param_1)

{
  int iVar1;
  undefined4 *puVar2;
  uint uVar3;
  uint uVar4;
  
  iVar1 = FUN_0003ff60();
  puVar2 = (undefined4 *)(**(code **)(*(int *)(iVar1 + 8) + 0x68))();
  *(undefined4 *)(param_1 + 0x56) = *puVar2;
  *(undefined4 *)(param_1 + 0x5a) = puVar2[1];
  *(undefined4 *)(param_1 + 0x4a) = *(undefined4 *)(param_1 + 0x56);
  *(undefined4 *)(param_1 + 0x4e) = *(undefined4 *)(param_1 + 0x5a);
  *(undefined4 *)(param_1 + 0x3e) = *(undefined4 *)(param_1 + 0x4a);
  *(undefined4 *)(param_1 + 0x42) = *(undefined4 *)(param_1 + 0x4e);
  *(undefined4 *)(param_1 + 0x32) = *(undefined4 *)(param_1 + 0x3e);
  *(undefined4 *)(param_1 + 0x36) = *(undefined4 *)(param_1 + 0x42);
  iVar1 = FUN_0003ff40();
  *(int *)(param_1 + 0x32) = *(int *)(param_1 + 0x32) - (iVar1 + 0x50);
  iVar1 = FUN_0003ff40();
  *(int *)(param_1 + 0x36) = *(int *)(param_1 + 0x36) + iVar1 + -5;
  iVar1 = FUN_0003ff40();
  *(int *)(param_1 + 0x4a) = *(int *)(param_1 + 0x4a) + iVar1 + 0x50;
  iVar1 = FUN_0003ff40();
  *(int *)(param_1 + 0x4e) = *(int *)(param_1 + 0x4e) + iVar1 + -5;
  iVar1 = FUN_0003ff40();
  *(int *)(param_1 + 0x3e) = *(int *)(param_1 + 0x3e) + iVar1 + -0x14;
  iVar1 = FUN_0003ff40();
  *(int *)(param_1 + 0x42) = *(int *)(param_1 + 0x42) - (iVar1 + 0x3c);
  switch(*(undefined4 *)(param_1 + 0x62)) {
  default:
    *(undefined4 *)(param_1 + 0x62) = 1;
    break;
  case 1:
    iVar1 = FUN_0003ff40();
    *(int *)(param_1 + 0x62) = iVar1 * 2;
    break;
  case 3:
    uVar3 = *(int *)(param_1 + 0x66) - 2;
    uVar4 = (int)uVar3 >> 0x1f;
    *(uint *)(param_1 + 0x62) = (uVar3 ^ uVar4) - uVar4;
  }
  return;
}

