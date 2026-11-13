
longlong __regparm3 FUN_00080fc0(int param_1,uint param_2)

{
  byte *pbVar1;
  undefined4 uVar2;
  int iVar3;
  undefined8 uVar4;
  undefined8 uVar5;
  
  if (*(int *)(param_1 + 0x2c) != -1) {
    uVar4 = (**(code **)(*(int *)(param_1 + 0x28) + 0x20))();
    iVar3 = (int)((ulonglong)uVar4 >> 0x20);
    pbVar1 = (byte *)(iVar3 + 0x39);
    *pbVar1 = *pbVar1 & 0xfe;
    *(undefined4 *)(iVar3 + 0x2c) = 0xffffffff;
    uVar5 = FUN_00081b76();
    if ((int)uVar5 == 0) {
      uVar2 = 0;
      if ((int)uVar4 != -1) {
        uVar2 = (int)((ulonglong)uVar5 >> 0x20);
      }
      return CONCAT44(param_2,uVar2);
    }
  }
  return (ulonglong)param_2 << 0x20;
}

