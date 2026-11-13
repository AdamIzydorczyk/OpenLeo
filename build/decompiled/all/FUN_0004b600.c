
undefined4 __regparm3 FUN_0004b600(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int *piVar1;
  int iVar2;
  code *pcVar3;
  int unaff_EBX;
  int *piVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  int aiStack_30 [4];
  undefined **local_20;
  undefined4 local_1c;
  int local_18;
  int local_10;
  
  aiStack_30[2] = 0;
  aiStack_30[3] = 0;
  local_20 = &PTR_LAB_00098278;
  local_1c = param_2;
  local_18 = param_3;
  FUN_000541f0();
  aiStack_30[2] = aiStack_30[2] + *(int *)(unaff_EBX + 0xca);
  aiStack_30[3] = aiStack_30[3] + *(int *)(unaff_EBX + 0xce);
  if (*(int **)(unaff_EBX + 0xa9) != (int *)0x0) {
    (**(code **)(**(int **)(unaff_EBX + 0xa9) + 8))();
  }
  piVar5 = aiStack_30 + 2;
  iVar6 = 0;
  if (0 < *(int *)(local_10 + 0x80)) {
    iVar7 = 0;
    piVar4 = aiStack_30 + 2;
    do {
      iVar2 = *(int *)(*(int *)((int)piVar4 + 0x18) + 0x74);
      *(undefined4 *)((int)piVar4 + -4) = 0;
      pcVar3 = *(code **)(*(int *)(iVar2 + iVar7) + 0x1c);
      *(undefined4 *)((int)piVar4 + -8) = 0x4b69a;
      (*pcVar3)();
      piVar5 = (int *)((int)piVar4 + -4);
      piVar1 = (int *)((int)piVar4 + 0x14);
      iVar6 = iVar6 + 1;
      iVar7 = iVar7 + 0xc;
      piVar4 = (int *)((int)piVar4 + -4);
    } while (iVar6 < *(int *)(*piVar1 + 0x80));
  }
  *(undefined4 *)((int)piVar5 + -4) = 0;
  *(undefined4 *)((int)piVar5 + -8) = 0x4b6c5;
  FUN_0004b500();
  return *(undefined4 *)((int)piVar5 + 8);
}

