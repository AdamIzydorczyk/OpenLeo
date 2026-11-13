
void __regparm3 FUN_0007ed1c(undefined4 param_1,undefined4 param_2)

{
  undefined4 extraout_ECX;
  undefined8 *extraout_EDX;
  undefined8 uVar1;
  undefined8 *puVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  
  uVar5 = 0x3fff;
  uVar4 = 0x80000000;
  uVar3 = 0;
  FUN_0007dee2(0,0x80000000,0x3fff,param_1,param_2);
  FUN_0007ed78(uVar3,uVar5,extraout_ECX,uVar4);
  puVar2 = extraout_EDX;
  FUN_0007e2ec();
  uVar1 = FUN_0007e316();
  *puVar2 = uVar1;
  *(undefined2 *)(puVar2 + 1) = 0x3fff;
  return;
}

