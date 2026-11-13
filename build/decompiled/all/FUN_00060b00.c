
void __regparm3 FUN_00060b00(int param_1,int *param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  int unaff_EBX;
  
  iVar1 = *(int *)(param_1 + 0x84);
  if ((*(int *)(*(int *)(param_1 + 0x2c) + unaff_EBX * 4) == iVar1) && (param_3 != 0)) {
    *(int *)(iVar1 + 0x30) = *(int *)(iVar1 + 0x30) + 1;
    *(int *)(iVar1 + 0x30) = *(int *)(iVar1 + 0x30) % 2;
  }
  iVar1 = *(int *)(param_1 + 0x88);
  if ((*(int *)(*(int *)(param_1 + 0x2c) + unaff_EBX * 4) == iVar1) && (param_3 != 0)) {
    *(int *)(iVar1 + 0x30) = *(int *)(iVar1 + 0x30) + 1;
    *(int *)(iVar1 + 0x30) = *(int *)(iVar1 + 0x30) % 2;
    if (*(int *)(*(int *)(param_1 + 0x88) + 0x30) == 0) {
      *(undefined4 *)(*(int *)(param_1 + 0x8c) + 0x2c) = 2;
    }
    else {
      *(undefined4 *)(*(int *)(param_1 + 0x8c) + 0x2c) = 1;
    }
  }
  if (*(int *)(param_1 + 0x90) == *(int *)(*(int *)(param_1 + 0x2c) + unaff_EBX * 4)) {
    FUN_0001c530();
    FUN_0001c530();
    iVar1 = *(int *)(*(int *)(param_1 + 0x90) + 0x10);
    (*(code *)PTR_FUN_0009827c)();
    (**(code **)(iVar1 + 8))();
    FUN_0001cc10();
    if (((DAT_000ceb90 & 1) != 0) &&
       (uVar2 = param_2[1] - *(int *)(*(int *)(param_1 + 0x90) + 0x34), uVar3 = (int)uVar2 >> 0x1f,
       (int)((uVar2 ^ uVar3) - uVar3) < 5)) {
      iVar1 = *(int *)(param_1 + 0x90);
      if ((*(int *)(iVar1 + 0x30) <= *param_2) &&
         (*param_2 <= *(int *)(iVar1 + 0x3c) + *(int *)(iVar1 + 0x30))) {
        iVar1 = *(int *)(iVar1 + 0x10);
        (*(code *)PTR_FUN_0009827c)();
        (**(code **)(iVar1 + 8))();
      }
    }
  }
  if (*(int *)(param_1 + 0x94) == *(int *)(*(int *)(param_1 + 0x2c) + unaff_EBX * 4)) {
    FUN_0001c530();
    FUN_0001c530();
    iVar1 = *(int *)(*(int *)(param_1 + 0x94) + 0x10);
    (*(code *)PTR_FUN_0009827c)();
    (**(code **)(iVar1 + 8))();
    FUN_00056380();
    if (((DAT_000ceb90 & 1) != 0) &&
       (uVar2 = param_2[1] - *(int *)(*(int *)(param_1 + 0x94) + 0x34), uVar3 = (int)uVar2 >> 0x1f,
       (int)((uVar2 ^ uVar3) - uVar3) < 5)) {
      iVar1 = *(int *)(param_1 + 0x94);
      if ((*(int *)(iVar1 + 0x30) <= *param_2) &&
         (*param_2 <= *(int *)(iVar1 + 0x30) + *(int *)(iVar1 + 0x3c))) {
        iVar1 = *(int *)(iVar1 + 0x10);
        (*(code *)PTR_FUN_0009827c)();
        (**(code **)(iVar1 + 8))();
      }
    }
  }
  return;
}

