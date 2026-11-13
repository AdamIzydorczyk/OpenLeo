
undefined8 __regparm3 FUN_00075874(int param_1,undefined4 param_2,undefined4 param_3)

{
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 8);
  if (iVar1 == 0) {
    if ((*(byte *)(param_1 + 4) & 1) != 0) {
      FUN_00077bdc();
      iVar1 = extraout_ECX;
    }
  }
  else {
    switch(*(undefined1 *)(iVar1 + 0x1c)) {
    case 0:
      *(char **)(param_1 + 0xc) = s_throw_during_destructor_while_un_00095ce3;
      FUN_00075384(param_2,param_3);
    case 3:
      FUN_00077bdc();
    case 5:
      FUN_00077bdc();
    case 6:
      FUN_00077bdc();
    default:
      FUN_0007539b();
      iVar1 = extraout_ECX_00;
      break;
    case 1:
    case 2:
      break;
    }
  }
  *(undefined4 *)(param_1 + 0xc) = 0;
  return CONCAT44(param_2,iVar1);
}

