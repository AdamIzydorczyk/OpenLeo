
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __regparm3 FUN_00049bd0(int param_1,int param_2,undefined4 param_3)

{
  short sVar1;
  short *psVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  short sVar6;
  int iVar7;
  uint uVar8;
  ushort unaff_BX;
  
  DAT_000cef18 = 0;
  FUN_000749a0();
  do {
    psVar2 = (short *)((uint)DAT_000cef18 * 8 + param_2);
    DAT_000cef1a = **(short **)(psVar2 + 2);
    DAT_000cef1c = (*(short **)(psVar2 + 2))[1];
    DAT_000cef20 = (DAT_000cef1a >> 1) + *psVar2 + -0x140;
    DAT_000cef1e = (DAT_000cef1a >> 1) - *psVar2;
    DAT_000cef22 = (DAT_000cef1c >> 1) - psVar2[1];
    DAT_000cef24 = psVar2[1] + (DAT_000cef1c >> 1) + -200;
    sVar1 = DAT_000cef1e;
    if (DAT_000cef1e < 1) {
      sVar1 = 0;
    }
    sVar6 = DAT_000cef20;
    if (DAT_000cef20 < 1) {
      sVar6 = 0;
    }
    DAT_000cef26 = DAT_000cef1a - (sVar1 + sVar6);
    iVar3 = (int)DAT_000cef22;
    if (iVar3 < 1) {
      iVar3 = 0;
    }
    iVar7 = (int)DAT_000cef24;
    if (iVar7 < 1) {
      iVar7 = 0;
    }
    uVar8 = (int)DAT_000cef1c - (iVar3 + iVar7);
    _DAT_000cef28 = (undefined2)uVar8;
    iVar3 = -(int)DAT_000cef22;
    if (iVar3 < 1) {
      iVar3 = 0;
    }
    iVar7 = -(int)DAT_000cef1e;
    if (iVar7 < 1) {
      iVar7 = 0;
    }
    iVar4 = (int)DAT_000cef22;
    if (iVar4 < 1) {
      iVar4 = 0;
    }
    iVar5 = (int)DAT_000cef1e;
    if (iVar5 < 1) {
      iVar5 = 0;
    }
    FUN_0001df3c(iVar5 + DAT_000cef1a * iVar4 + *(int *)((uint)DAT_000cef18 * 8 + param_2 + 4) + 4,
                 iVar7 + *(int *)(param_1 + 6) + iVar3 * 0x140,DAT_000cef26,
                 (int)DAT_000cef1a - (uint)DAT_000cef26,uVar8 & 0xffff);
    DAT_000cef18 = DAT_000cef18 + 1;
  } while (DAT_000cef18 < unaff_BX);
  FUN_0001df81(*(undefined4 *)(param_1 + 6),param_3);
  return;
}

