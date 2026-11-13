
void __regparm3 FUN_000281b0(int *param_1)

{
  int iVar1;
  longdouble lVar2;
  int iVar3;
  undefined4 uVar4;
  
  uVar4 = 0;
  if ((param_1[0x17] == 4) && (*(int *)((int)param_1 + 0x112) == 0)) {
    iVar1 = (**(code **)(param_1[2] + 0x70))();
    if (iVar1 == 1) {
      iVar1 = param_1[5];
      if (*(int *)((int)param_1 + 0x126) <= param_1[5]) {
        iVar1 = *(int *)((int)param_1 + 0x126);
      }
      if (*param_1 <= iVar1) {
        iVar1 = 0;
        iVar3 = 0;
        goto LAB_0002829a;
      }
    }
  }
  (**(code **)(param_1[2] + 0x68))();
  (**(code **)(param_1[2] + 0x68))();
  lVar2 = (longdouble)FUN_00075d96();
  iVar1 = (int)ROUND(lVar2);
  lVar2 = (longdouble)FUN_00075d96(iVar1);
  iVar3 = (int)ROUND(lVar2);
LAB_0002829a:
  (**(code **)(param_1[2] + 0xcc))(iVar1,iVar3,uVar4);
  FUN_000550a0();
  return;
}

