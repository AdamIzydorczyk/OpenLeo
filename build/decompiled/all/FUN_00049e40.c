
void __regparm3 FUN_00049e40(undefined4 param_1,int param_2)

{
  ushort uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  
  iVar4 = param_2 + 0x180;
  for (uVar1 = 0; uVar1 < 0x40; uVar1 = uVar1 + 1) {
    FUN_00049db0();
  }
  iVar7 = param_2 + 0x180;
  iVar2 = param_2 + 0x240;
  for (uVar1 = 0; uVar1 < 0x20; uVar1 = uVar1 + 1) {
    FUN_00049db0();
  }
  iVar3 = param_2 + 0x240;
  iVar5 = param_2 + 0x2a0;
  for (uVar1 = 0; uVar1 < 0x10; uVar1 = uVar1 + 1) {
    FUN_00049db0();
  }
  iVar6 = param_2 + 0x2a0;
  iVar8 = param_2 + 0x2d0;
  for (uVar1 = 0; uVar1 < 8; uVar1 = uVar1 + 1) {
    FUN_00049db0();
  }
  param_2 = param_2 + 0x2d0;
  for (uVar1 = 0; uVar1 < 4; uVar1 = uVar1 + 1) {
    FUN_00049db0(param_2,iVar2,iVar3,iVar4,iVar5,iVar6,iVar7,iVar8);
  }
  return;
}

