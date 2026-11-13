
undefined4 __regparm1 FUN_00075f7e(undefined4 param_1,int param_2)

{
  undefined1 uVar1;
  byte bVar2;
  undefined2 uVar3;
  int extraout_ECX;
  undefined4 extraout_ECX_00;
  uint unaff_EBX;
  undefined8 uVar4;
  undefined4 uVar5;
  
  *(byte *)(param_2 + 0xc) = *(byte *)(param_2 + 0xc) & 0xfc;
  *(uint *)(param_2 + 0xc) = *(uint *)(param_2 + 0xc) | unaff_EBX;
  uVar4 = FUN_00078c41();
  if (((uint)uVar4 & 0xff) == 0x72) {
    uVar1 = 0;
    if ((unaff_EBX & 2) != 0) {
      uVar1 = 2;
    }
    if ((unaff_EBX & 0x40) == 0) {
      uVar3 = CONCAT11(1,uVar1);
    }
    else {
      uVar3 = CONCAT11(2,uVar1);
    }
    uVar5 = 0;
  }
  else {
    bVar2 = ((unaff_EBX & 1) != 0) + 0x21;
    if ((unaff_EBX & 0x80) == 0) {
      bVar2 = bVar2 | 0x40;
    }
    else {
      bVar2 = bVar2 | 0x10;
    }
    if ((unaff_EBX & 0x40) == 0) {
      uVar3 = CONCAT11(1,bVar2);
    }
    else {
      uVar3 = CONCAT11(2,bVar2);
    }
    uVar5 = 0x180;
  }
  uVar5 = FUN_0007a0cc(param_1,uVar3,(int)((ulonglong)uVar4 >> 0x20),uVar5);
  *(undefined4 *)(extraout_ECX + 0x10) = uVar5;
  if (*(int *)(extraout_ECX + 0x10) == -1) {
    FUN_0007a3c0();
    return 0;
  }
  *(undefined4 *)(extraout_ECX + 4) = 0;
  *(undefined4 *)(extraout_ECX + 8) = 0;
  *(undefined4 *)(extraout_ECX + 0x14) = 0;
  if ((unaff_EBX & 0x80) != 0) {
    FUN_000765ab();
  }
  FUN_0007a417();
  return extraout_ECX_00;
}

