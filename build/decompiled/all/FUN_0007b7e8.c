
undefined8 __regparm3 FUN_0007b7e8(char *param_1,undefined4 param_2)

{
  undefined4 extraout_EDX;
  
  if (*param_1 == '\x02') {
    param_1 = *(char **)(param_1 + 1);
  }
  switch(*param_1) {
  case '\0':
  case '\x04':
    return CONCAT44(param_2,*(undefined4 *)(param_1 + 0xd));
  case '\x01':
  case '\x06':
  case '\a':
    return CONCAT44(param_2,4);
  default:
    FUN_0007539b();
    return CONCAT44(param_2,extraout_EDX);
  case '\x03':
  case '\b':
    return CONCAT44(param_2,(uint)(byte)param_1[1]);
  }
}

