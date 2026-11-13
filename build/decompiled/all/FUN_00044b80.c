
void __regparm3 FUN_00044b80(int *param_1,undefined1 *param_2)

{
  undefined1 uVar1;
  int iVar2;
  int *extraout_ECX;
  int *extraout_ECX_00;
  int *piVar3;
  undefined1 *extraout_EDX;
  undefined1 *puVar5;
  undefined1 *puVar4;
  ulonglong uVar6;
  
  piVar3 = param_1 + 3;
  puVar4 = param_2;
  while ((int)puVar4 - (int)param_2 < *param_1) {
    if (*(int **)((int)piVar3 + 1) == (int *)0x0) {
      uVar6 = CONCAT44(puVar4,0xffffffff);
    }
    else {
      uVar6 = (**(code **)(**(int **)((int)piVar3 + 1) + 0x24))();
      piVar3 = extraout_ECX;
    }
    puVar5 = (undefined1 *)(uVar6 >> 0x20);
    if (((uint)uVar6 & 0xc0) == 0xc0) {
      if (*(int **)((int)piVar3 + 1) == (int *)0x0) {
        uVar1 = 0xff;
        puVar4 = puVar5;
      }
      else {
        uVar1 = (**(code **)(**(int **)((int)piVar3 + 1) + 0x24))();
        piVar3 = extraout_ECX_00;
        puVar4 = extraout_EDX;
      }
      iVar2 = 0;
      puVar5 = puVar4;
      if ((uVar6 & 0x3f) != 0) {
        do {
          puVar4 = puVar5 + 1;
          iVar2 = iVar2 + 1;
          *puVar5 = uVar1;
          puVar5 = puVar4;
        } while (iVar2 < (int)((uint)uVar6 & 0x3f));
      }
    }
    else {
      puVar4 = puVar5 + 1;
      *puVar5 = (char)uVar6;
    }
  }
  return;
}

