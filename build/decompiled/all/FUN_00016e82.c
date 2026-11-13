
undefined4 __regparm3 FUN_00016e82(int param_1,uint param_2)

{
  int iVar1;
  uint uVar2;
  uint extraout_ECX;
  int iVar3;
  undefined8 uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  
  iVar6 = param_2 + 8;
  iVar7 = param_2 + 4;
  iVar3 = 0;
  uVar5 = param_1 + 0x1fU & 0xffffffe0;
  if (DAT_000a886d == 0) {
    DAT_000a886d = param_2;
    FUN_00016ce1();
    FUN_00016ce1();
    FUN_00016ce1();
  }
  else {
    uVar2 = DAT_000a886d;
    iVar8 = iVar7;
    while ((uVar2 != 0 && (iVar3 == 0))) {
      iVar3 = FUN_00016d17(uVar5,iVar6,iVar7,iVar8);
      iVar1 = FUN_00016d17();
      if (param_2 < extraout_ECX) {
        if (iVar1 == 0) {
          DAT_000a886d = param_2;
        }
        else {
          FUN_00016ce1();
        }
        FUN_00016ce1();
        FUN_00016ce1();
        FUN_00016ce1();
LAB_00016f62:
        FUN_00016ce1();
      }
      else if (iVar3 == 0) {
        FUN_00016ce1();
        FUN_00016ce1();
        FUN_00016ce1();
        goto LAB_00016f62;
      }
      uVar4 = FUN_00016d17();
      uVar2 = (uint)uVar4;
      iVar3 = (int)((ulonglong)uVar4 >> 0x20);
    }
    if (iVar3 == 0) {
      return 4;
    }
  }
  FUN_00016e13();
  return 1;
}

