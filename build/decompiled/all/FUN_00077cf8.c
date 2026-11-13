
undefined8 __regparm3 FUN_00077cf8(int *param_1,undefined4 param_2)

{
  ushort uVar1;
  ushort *puVar2;
  char cVar3;
  int iVar4;
  int extraout_ECX;
  int *local_1c;
  
  if (*param_1 == 0) {
    local_1c = (int *)0x0;
  }
  else {
    iVar4 = param_1[1];
    if (iVar4 != 0) {
      iVar4 = iVar4 + -1;
    }
    while (iVar4 != 0) {
      if ((iVar4 == *(int *)((int)param_1 + 9)) ||
         (local_1c = (int *)FUN_00077ca4(), iVar4 = extraout_ECX, *local_1c != 0))
      goto LAB_00077d50;
      puVar2 = (ushort *)local_1c[1];
      uVar1 = *puVar2;
      if (uVar1 == 0) goto LAB_00077d50;
      if (uVar1 < 2) {
        iVar4 = *(int *)(puVar2 + 1);
      }
      else {
        if (uVar1 != 5) goto LAB_00077d50;
        cVar3 = FUN_0007be5c();
        if (cVar3 == '\0') {
          iVar4 = *(int *)(puVar2 + 5);
        }
        else {
          iVar4 = *(int *)(puVar2 + 3);
        }
      }
    }
    local_1c = (int *)0x0;
LAB_00077d50:
    param_1[1] = iVar4;
  }
  return CONCAT44(param_2,local_1c);
}

