
int __regparm3 FUN_000498d0(int param_1)

{
  int iVar1;
  int extraout_ECX;
  
  iVar1 = (**(code **)**(undefined4 **)(param_1 + 1))();
  if (iVar1 == -1) {
    *(undefined4 *)(extraout_ECX + 0xd) = 1;
  }
  return iVar1;
}

