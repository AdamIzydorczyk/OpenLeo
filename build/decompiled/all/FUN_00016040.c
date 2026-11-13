
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __regparm3 FUN_00016040(int param_1,uint param_2)

{
  byte bVar1;
  undefined4 uVar2;
  uint uVar3;
  undefined2 extraout_var;
  int iVar4;
  int iVar5;
  
  while ((short)param_2 != 0) {
    if (DAT_000c3f74 == 0) {
      (*(code *)PTR_FUN_000a71b4)(param_2,param_1);
      DAT_000c3f74 = (ushort)((CONCAT22(extraout_var,_DAT_000a71ac) * 0x7d) /
                             (int)((uint)DAT_000a71b0 * 0x32));
      iVar4 = 0;
      for (iVar5 = 0; iVar5 < (int)(uint)DAT_000ae674; iVar5 = iVar5 + 1) {
        if ((&DAT_000b33d0)[iVar4] != '\0') {
          (&DAT_000b33d0)[iVar4] = 0;
          *(int *)((int)&DAT_000b33ec + iVar4) = *(int *)((int)&DAT_000b33d6 + iVar4) << 0xb;
          (&DAT_000b33d1)[iVar4] = 1;
        }
        if (*(int *)((int)&DAT_000b33e6 + iVar4) == 0) {
          (&DAT_000b33d1)[iVar4] = 0;
        }
        if ((&DAT_000b33d1)[iVar4] != '\0') {
          uVar2 = FUN_00015aa8();
          *(undefined4 *)(iVar4 + 0xb33f0) = uVar2;
          if ((*(byte *)((int)&DAT_000b33d2 + iVar4) & 0x80) != 0) {
            *(int *)(iVar4 + 0xb33f0) = -*(int *)(iVar4 + 0xb33f0);
          }
          bVar1 = (&DAT_000b33eb)[iVar4];
          uVar3 = (uint)(byte)(&DAT_000b33ea)[iVar4];
          if ((DAT_000ae672 & 1) == 0) {
            *(undefined2 *)(iVar4 + 0xb33f4) = (&DAT_000c3cf0)[uVar3];
          }
          else {
            *(undefined2 *)(iVar4 + 0xb33f4) =
                 (&DAT_000c3cf0)[(int)((0xff - (uint)bVar1) * uVar3) / 0xff];
            *(undefined2 *)(iVar4 + 0xb33f6) = (&DAT_000c3cf0)[(uVar3 * bVar1) / 0xff];
          }
        }
        iVar4 = iVar4 + 0x28;
      }
    }
    if (DAT_000c3f74 < (ushort)param_2) {
      uVar3 = (uint)DAT_000c3f74;
    }
    else {
      uVar3 = param_2;
    }
    param_2 = param_2 - uVar3;
    FUN_00015ff0();
    DAT_000c3f74 = DAT_000c3f74 - (short)uVar3;
    uVar3 = FUN_00015ad0();
    param_1 = param_1 + (uVar3 & 0xffff);
  }
  return;
}

