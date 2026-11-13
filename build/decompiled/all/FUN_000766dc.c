
int * __regparm3 FUN_000766dc(int *param_1,char *param_2)

{
  char cVar1;
  int iVar2;
  byte bVar3;
  undefined8 uVar4;
  
  bVar3 = 0;
  if (param_2 != (char *)0x0) {
    if (*(int *)((int)param_1 + *(int *)(*param_1 + 4) + 0x10) == 0) {
      if ((*(int *)((int)param_1 + *(int *)(*param_1 + 4) + 8) == 0) &&
         ((*(uint *)((int)param_1 + *(int *)(*param_1 + 4) + 0xc) & 0x4000) == 0)) {
        iVar2 = 1;
      }
      else {
        uVar4 = FUN_0007a648();
        param_2 = (char *)((ulonglong)uVar4 >> 0x20);
        iVar2 = (int)uVar4;
      }
    }
    else {
      iVar2 = 0;
    }
    if (iVar2 != 0) {
      iVar2 = -1;
      do {
        if (iVar2 == 0) break;
        iVar2 = iVar2 + -1;
        cVar1 = *param_2;
        param_2 = param_2 + (uint)bVar3 * -2 + 1;
      } while (cVar1 != '\0');
      FUN_0007a69c();
      FUN_0007a9c0();
      if ((*(uint *)((int)param_1 + *(int *)(*param_1 + 4) + 0xc) & 0x2000) != 0) {
        FUN_0007a9fc();
      }
    }
  }
  return param_1;
}

