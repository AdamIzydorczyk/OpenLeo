
void __regparm3 FUN_00058140(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  int local_14;
  undefined1 local_10 [4];
  undefined4 uStack_c;
  undefined4 uStack_8;
  
  uStack_c = param_2;
  uStack_8 = param_3;
  FUN_0001ded4(&local_14,local_10,1,2);
  *(uint *)(param_1 + 0x14) = (uint)(local_14 != 0x3fff);
  if ((local_14 != 0x3fff) == 0) {
    FUN_0001ded4(&local_14,local_10,4,8);
    *(uint *)(param_1 + 0x18) = (uint)(local_14 != 0x3fff);
  }
  if ((*(int *)(param_1 + 0x14) == 0) && (*(int *)(param_1 + 0x18) == 0)) {
    uVar1 = 0;
  }
  else {
    uVar1 = 1;
  }
  *(undefined4 *)(param_1 + 0x10) = uVar1;
  return;
}

