
void __regparm3 FUN_00045120(int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int iVar2;
  int iVar3;
  undefined1 local_20;
  char local_1c [4];
  undefined1 local_18;
  
  FUN_00049920(param_2,&DAT_0009182d,param_1 + 8);
  if (*(int *)(extraout_ECX + 8) != 0) {
    iVar3 = 1;
    thunk_FUN_0005c320();
    uVar1 = FUN_00076bb7();
    *(undefined4 *)(extraout_ECX_00 + 4) = uVar1;
    FUN_0004a0b0();
    if (0 < *(int *)(extraout_ECX_01 + 8)) {
      iVar2 = 0;
      do {
        FUN_00049920(param_2,&DAT_00091830,local_1c);
        FUN_0004a0b0();
        if (local_1c[0] == ';') {
          local_20 = 1;
        }
        else if (local_1c[0] == '#') {
          local_20 = 2;
        }
        else {
          local_20 = 0;
        }
        local_18 = local_20;
        *(undefined1 *)(iVar2 + 0xc + *(int *)(extraout_ECX_02 + 4)) = local_20;
        iVar3 = iVar3 + 1;
        iVar2 = iVar2 + 0xd;
      } while (iVar3 <= *(int *)(extraout_ECX_02 + 8));
    }
    FUN_000498f0(param_2,&DAT_00091834);
  }
  return;
}

