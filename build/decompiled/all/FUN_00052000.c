
void __regparm3 FUN_00052000(int param_1,undefined4 param_2,undefined4 *param_3)

{
  int iVar1;
  int extraout_ECX;
  undefined4 *unaff_EBX;
  int iVar2;
  
  *unaff_EBX = 0;
  iVar1 = 0;
  iVar2 = 0;
  do {
    if (*(int *)(param_1 + 0x22) <= iVar1) {
LAB_0005204f:
      (**(code **)(*(int *)(param_1 + 0x44) + 4))();
      return;
    }
    iVar1 = FUN_00047e40(param_3,param_2);
    if (iVar1 != 0) {
      *unaff_EBX = *(undefined4 *)(iVar2 + 0xd + *(int *)(param_1 + 0x1a));
      *param_3 = *(undefined4 *)(iVar2 + 0x11 + *(int *)(param_1 + 0x1a));
      goto LAB_0005204f;
    }
    iVar2 = iVar2 + 0x15;
    iVar1 = extraout_ECX + 1;
  } while( true );
}

