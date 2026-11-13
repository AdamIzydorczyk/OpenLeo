
void __regparm3 FUN_00040810(int *param_1,undefined4 param_2)

{
  int iVar1;
  undefined8 uVar2;
  
  if (*param_1 == 0) {
    *(undefined1 *)(param_1 + 1) = 0;
    return;
  }
  FUN_00040770(param_2);
  FUN_00040710();
  FUN_00040730();
  uVar2 = FUN_00040730();
  iVar1 = (int)((ulonglong)uVar2 >> 0x20);
  if (iVar1 == 1) {
    *(undefined1 *)(param_1 + 1) = 1;
    return;
  }
  if (iVar1 == 2) {
    *(undefined1 *)(param_1 + 1) = 2;
    return;
  }
  if (iVar1 != 3) {
    *(undefined1 *)(param_1 + 1) = 5;
    return;
  }
  if ((int)uVar2 == 0) {
    *(undefined1 *)(param_1 + 1) = 3;
    return;
  }
  *(undefined1 *)(param_1 + 1) = 4;
  return;
}

