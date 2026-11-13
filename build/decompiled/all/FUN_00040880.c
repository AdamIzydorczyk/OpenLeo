
void __regparm3 FUN_00040880(int *param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 *puVar2;
  
  if (*param_1 == 0) {
    puVar2 = (undefined4 *)FUN_0005c320(param_2);
    if (puVar2 != (undefined4 *)0x0) {
      *puVar2 = &PTR_LAB_000a0b60;
    }
  }
  else if ((char)param_1[1] < '\x03') {
    iVar1 = FUN_0005c320(param_2);
    puVar2 = (undefined4 *)0x0;
    if (iVar1 != 0) {
      puVar2 = (undefined4 *)FUN_00040290();
    }
  }
  else if ((char)param_1[1] < '\x05') {
    iVar1 = FUN_0005c320(param_2);
    puVar2 = (undefined4 *)0x0;
    if (iVar1 != 0) {
      puVar2 = (undefined4 *)FUN_000402f0();
    }
  }
  else {
    iVar1 = FUN_0005c320(param_2);
    puVar2 = (undefined4 *)0x0;
    if (iVar1 != 0) {
      puVar2 = (undefined4 *)FUN_00040430();
    }
  }
  *(undefined4 **)((int)param_1 + 5) = puVar2;
  return;
}

