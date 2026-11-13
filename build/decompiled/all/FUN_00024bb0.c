
undefined8 __regparm3 FUN_00024bb0(int param_1,undefined4 param_2)

{
  int *piVar1;
  undefined4 uVar2;
  
  piVar1 = (int *)(**(code **)(*(int *)(*(int *)(DAT_000cef40 + 0x394) + 8) + 0x68))();
  if ((((*piVar1 < *(int *)(param_1 + 0xf2)) || (*(int *)(param_1 + 0xfa) < *piVar1)) ||
      (piVar1[1] < *(int *)(param_1 + 0xf6))) || (*(int *)(param_1 + 0xfe) < piVar1[1])) {
    uVar2 = 0;
  }
  else {
    uVar2 = 1;
  }
  return CONCAT44(param_2,uVar2);
}

