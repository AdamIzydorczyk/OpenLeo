
undefined8 __regparm3 FUN_00075ead(int param_1,undefined4 param_2)

{
  char cVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  bool bVar5;
  undefined8 uVar6;
  
  uVar6 = FUN_00078c41();
  uVar3 = (uint)((ulonglong)uVar6 >> 0x20);
  uVar2 = (uint)uVar6 & 0xff;
  if (((uVar2 != 0x72) && (uVar2 != 0x77)) && (uVar2 != 0x61)) {
    FUN_00078c12();
    uVar3 = 0;
    goto LAB_00075f76;
  }
  cVar1 = *(char *)(param_1 + 1);
  uVar2 = uVar3 | 3;
  if (cVar1 == '+') {
    uVar4 = CONCAT22((short)((ulonglong)uVar6 >> 0x30),(short)uVar2) | 0x40;
    if (*(char *)(param_1 + 2) == 'b') {
LAB_00075f1a:
      uVar3 = uVar4;
    }
    else {
      uVar3 = uVar2;
      if (*(char *)(param_1 + 2) != 't') {
        bVar5 = DAT_000a9e09 == 0x200;
        goto LAB_00075f18;
      }
    }
  }
  else {
    uVar4 = uVar3 | 0x40;
    if (cVar1 == 'b') {
      uVar3 = uVar4;
      if (*(char *)(param_1 + 2) == '+') {
        uVar4 = CONCAT31((int3)((ulonglong)uVar6 >> 0x28),(char)uVar4) | 3;
LAB_00075f56:
        uVar3 = uVar4;
      }
    }
    else if (cVar1 == 't') {
      bVar5 = *(char *)(param_1 + 2) == '+';
      uVar4 = uVar2;
LAB_00075f18:
      if (bVar5) goto LAB_00075f1a;
    }
    else if (DAT_000a9e09 == 0x200) goto LAB_00075f56;
  }
  uVar2 = (uint)uVar6 & 0xff;
  if (uVar2 == 0x77) {
    uVar3 = uVar3 | 2;
  }
  else if (uVar2 == 0x61) {
    uVar3 = uVar3 | 0x82;
  }
  else {
    uVar3 = uVar3 | 1;
  }
LAB_00075f76:
  return CONCAT44(param_2,uVar3);
}

