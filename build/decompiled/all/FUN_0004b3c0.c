
void __regparm3 FUN_0004b3c0(int param_1)

{
  int iVar1;
  int extraout_ECX;
  int *extraout_ECX_00;
  int *piVar2;
  undefined2 *unaff_EBX;
  undefined4 *puVar3;
  byte bVar4;
  undefined4 local_14;
  undefined4 local_10;
  
  bVar4 = 0;
  if (*(int *)(*(int *)(param_1 + 0x394) + 0x5c) != 2) {
    local_14 = CONCAT22(unaff_EBX[2],*unaff_EBX);
    iVar1 = FUN_0004b3b0();
    local_10 = *(undefined4 *)(iVar1 + 0xc);
    piVar2 = (int *)(extraout_ECX + 0x3ce);
    if (*(int *)(extraout_ECX + 0x3d6) == *(int *)(extraout_ECX + 0x3da)) {
      FUN_00022270();
      piVar2 = extraout_ECX_00;
    }
    puVar3 = (undefined4 *)(*piVar2 + piVar2[2] * 8);
    piVar2[2] = piVar2[2] + 1;
    *puVar3 = local_14;
    puVar3[(uint)bVar4 * -2 + 1] = (&local_10)[(uint)bVar4 * -2];
  }
  return;
}

