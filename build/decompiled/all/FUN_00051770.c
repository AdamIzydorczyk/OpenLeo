
undefined4 __regparm3 FUN_00051770(undefined4 *param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 extraout_ECX;
  
  param_1[1] = 0;
  *(undefined4 *)((int)param_1 + 0xe) = 1000;
  *(undefined4 *)((int)param_1 + 0x12) = 0;
  *param_1 = param_2;
  *(undefined4 *)((int)param_1 + 0x16) = 0;
  FUN_00022220();
  iVar1 = FUN_00021750();
  *(undefined ***)(iVar1 + 0x16) = &PTR_FUN_000a260c;
  FUN_00051eb0();
  return extraout_ECX;
}

