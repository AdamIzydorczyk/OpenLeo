
/* WARNING: Removing unreachable block (ram,0x0004acbd) */
/* WARNING: Removing unreachable block (ram,0x0004acd2) */
/* WARNING: Removing unreachable block (ram,0x0004ad2d) */
/* WARNING: Removing unreachable block (ram,0x0004ad40) */
/* WARNING: Removing unreachable block (ram,0x0004adca) */
/* WARNING: Removing unreachable block (ram,0x0004ae4e) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 __regparm3 FUN_0004abd0(int param_1,undefined4 param_2)

{
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int iVar1;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int extraout_ECX_06;
  undefined4 uVar2;
  
  (**(code **)(*(int *)(*(int *)(param_1 + 0x394) + 8) + 0x54))();
  (**(code **)(*(int *)(*(int *)(extraout_ECX + 0x394) + 8) + 0xec))();
  (**(code **)(*(int *)(*(int *)(extraout_ECX_00 + 0x394) + 8) + 0xe8))();
  FUN_000576e0();
  iVar1 = extraout_ECX_01;
  do {
    if (*(int *)(*(int *)(iVar1 + 0x394) + 0x116) != 0) {
      uVar2 = 3;
LAB_0004ae53:
      FUN_0001cfd0();
      FUN_0001dba0();
      FUN_0001cfe5();
      FUN_0004a160();
      FUN_0004a9f0();
      return CONCAT44(param_2,uVar2);
    }
    if (*(int *)(iVar1 + 4) != 0) {
      uVar2 = 1;
      goto LAB_0004ae53;
    }
    FUN_00058690();
    if ((*(int *)(extraout_ECX_02 + 0x3a0) == 0) && (*(int *)(extraout_ECX_02 + 0x3b0) != 0)) {
      FUN_0004a8f0();
      (**(code **)(*(int *)(*(int *)(extraout_ECX_03 + 0x3b0) + 4) + 8))();
      *(undefined4 *)(extraout_ECX_04 + 0x3b0) = 0;
      FUN_0004a8c0();
    }
    if (DAT_000a94f0 == 0) {
      uVar2 = 2;
      goto LAB_0004ae53;
    }
    FUN_0004ab50();
    FUN_000222d0();
    FUN_0004b970();
    FUN_0004ba90();
    (**(code **)(*(int *)(*(int *)(extraout_ECX_05 + 0x394) + 8) + 0xcc))();
    FUN_00052b80();
    FUN_0004ab80();
    FUN_000563d0();
    FUN_0004af10();
    FUN_0004b0e0();
    _DAT_000cef3c = _DAT_000cef3c + 1;
    iVar1 = extraout_ECX_06;
  } while( true );
}

