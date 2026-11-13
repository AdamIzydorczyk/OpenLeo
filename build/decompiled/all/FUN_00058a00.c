
int * __regparm3 FUN_00058a00(int *param_1,int param_2,int param_3)

{
  int iVar1;
  int unaff_EBX;
  
  param_1[3] = 0;
  *param_1 = unaff_EBX;
  param_1[5] = param_2;
  iVar1 = *param_1;
  param_1[6] = param_3;
  if (199 < iVar1) {
    iVar1 = 200;
  }
  param_1[2] = iVar1;
  thunk_FUN_0005c320();
  iVar1 = FUN_00076ab0();
  param_1[4] = iVar1;
  param_1[1] = param_1[2];
  param_1[1] = -param_1[2];
  return param_1;
}

