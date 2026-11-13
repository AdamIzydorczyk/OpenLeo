
void __regparm3 FUN_00075cb9(undefined2 *param_1)

{
  undefined2 in_ES;
  undefined2 in_CS;
  undefined2 in_SS;
  undefined2 in_DS;
  undefined2 in_FS;
  undefined2 in_GS;
  
  param_1[1] = in_CS;
  param_1[3] = in_DS;
  *param_1 = in_ES;
  param_1[2] = in_SS;
  param_1[4] = in_FS;
  param_1[5] = in_GS;
  return;
}

