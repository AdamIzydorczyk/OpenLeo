
void __regparm3 FUN_0002de30(int param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar1 = param_1 + 0x54;
  iVar4 = 0;
  iVar5 = param_1 + 4;
  iVar2 = param_1 + 0xc;
  iVar3 = param_1 + 8;
  do {
    FUN_00049920(param_2,s_rect____d___d____d___d_00090b74,iVar4 * 0x1c + param_1,iVar5,iVar3,iVar2,
                 param_1,iVar2,iVar1);
    FUN_0004a0b0();
    iVar4 = iVar4 + 1;
    iVar3 = iVar3 + 0x1c;
    iVar5 = iVar5 + 0x1c;
    FUN_00049920(param_2,s_dir____d_00090b8e,iVar1);
    iVar1 = iVar1 + 4;
    iVar2 = iVar2 + 0x1c;
  } while (iVar4 < 3);
  return;
}

