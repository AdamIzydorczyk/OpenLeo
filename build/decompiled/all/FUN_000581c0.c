
void __regparm3 FUN_000581c0(int param_1,undefined4 param_2)

{
  uint uVar1;
  uint uVar2;
  undefined4 uVar3;
  
  if (*(int *)(param_1 + 0x14) == 0) {
    uVar2 = 0x40;
    uVar3 = 8;
  }
  else {
    uVar2 = 0x10;
    uVar3 = 2;
  }
  do {
    FUN_0001ded4(param_2);
    uVar1 = FUN_0001df34(uVar3);
  } while ((uVar1 & uVar2) == 0);
  return;
}

