
void __regparm3 FUN_0002da30(int *param_1)

{
  int iVar1;
  int iVar2;
  code *pcVar3;
  int iVar4;
  int extraout_ECX;
  int *extraout_EDX;
  undefined4 *extraout_EDX_00;
  int iVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  undefined4 auStack_24 [3];
  
  puVar7 = auStack_24 + 3;
  puVar6 = auStack_24 + 3;
  if (*param_1 != 0) {
    iVar5 = 0;
    if (0 < param_1[2]) {
      iVar4 = 0;
      do {
        iVar1 = *param_1;
        *(undefined4 *)((int)puVar6 + -4) = *(undefined4 *)(iVar4 + 4 + iVar1);
        iVar2 = param_1[4];
        *(undefined4 *)((int)puVar6 + -8) = *(undefined4 *)(iVar4 + iVar1);
        iVar5 = iVar5 + 1;
        pcVar3 = *(code **)(iVar2 + 4);
        *(undefined4 *)((int)puVar6 + -0xc) = 0x2da5c;
        (*pcVar3)();
        puVar7 = (undefined4 *)((int)puVar6 + -8);
        puVar6 = (undefined4 *)((int)puVar6 + -8);
        iVar4 = extraout_ECX + 8;
        param_1 = extraout_EDX;
      } while (iVar5 < extraout_EDX[2]);
    }
    *(undefined4 *)((int)puVar7 + -4) = 0x2da6d;
    thunk_FUN_0005c370();
    *extraout_EDX_00 = 0;
    extraout_EDX_00[2] = 0;
    extraout_EDX_00[3] = 0;
  }
  return;
}

