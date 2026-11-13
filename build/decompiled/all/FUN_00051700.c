
undefined8 __regparm3 FUN_00051700(undefined4 *param_1,undefined4 param_2)

{
  int iVar1;
  
  *param_1 = 0;
  param_1[1] = 0;
  *(undefined2 *)(param_1 + 2) = 0;
  *(undefined2 *)((int)param_1 + 10) = 0;
  *(undefined2 *)(param_1 + 3) = 0;
  *(undefined4 *)((int)param_1 + 0xe) = 1000;
  *(undefined4 *)((int)param_1 + 0x12) = 0;
  *(undefined4 *)((int)param_1 + 0x16) = 0xffffffff;
  FUN_00022220();
  iVar1 = FUN_00021750();
  *(undefined ***)(iVar1 + 0x16) = &PTR_FUN_000a260c;
  return CONCAT44(param_2,iVar1 + -0x2e);
}

