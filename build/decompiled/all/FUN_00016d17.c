
undefined8 __regparm3 FUN_00016d17(undefined4 param_1,undefined4 param_2)

{
  undefined1 uVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 local_1c;
  
  puVar2 = &local_1c;
  iVar3 = 0;
  do {
    uVar1 = FUN_00016c57();
    iVar3 = iVar3 + 1;
    *(undefined1 *)puVar2 = uVar1;
    puVar2 = (undefined4 *)((int)puVar2 + 1);
  } while (iVar3 < 4);
  return CONCAT44(param_2,local_1c);
}

