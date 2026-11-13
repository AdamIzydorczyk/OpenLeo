
uint * __regparm3 FUN_00049550(uint *param_1,uint *param_2)

{
  uint uVar1;
  uint *puVar2;
  uint *puVar3;
  
  puVar2 = param_1;
  do {
    uVar1 = *param_2;
    puVar3 = puVar2 + 1;
    param_2 = param_2 + 1;
    *puVar2 = *puVar2 & uVar1;
    puVar2 = puVar3;
  } while (puVar3 != param_1 + 3);
  return param_1;
}

