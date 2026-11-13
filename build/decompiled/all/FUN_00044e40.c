
int * __regparm3 FUN_00044e40(int *param_1,uint param_2)

{
  uint extraout_ECX;
  
  if ((param_2 & 4) == 0) {
    param_1[2] = (int)&PTR_FUN_00097f50;
    if (*param_1 != 0) {
      (*(code *)**(undefined4 **)(*param_1 + 8))();
      param_2 = extraout_ECX;
    }
    if ((param_2 & 2) != 0) {
      FUN_0005c370();
    }
  }
  else {
    FUN_00074bc0();
    thunk_FUN_0005c370();
  }
  return param_1;
}

