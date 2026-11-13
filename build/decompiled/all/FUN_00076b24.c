
undefined4 __regparm3
FUN_00076b24(undefined4 *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  char *unaff_EBX;
  undefined4 uVar1;
  code *local_c;
  
  if (param_1 == (undefined4 *)0x0) {
    uVar1 = 0;
  }
  else {
    *param_1 = param_2;
    FUN_00077e10(param_4,param_3,param_1);
    if (*unaff_EBX == '\x04') {
      local_c = FUN_0007b82c;
    }
    else {
      local_c = FUN_00076ab0;
    }
    uVar1 = (*local_c)();
    FUN_00077e29();
  }
  return uVar1;
}

