
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

char * __regparm3 FUN_0001b2b0(undefined4 param_1,char *param_2)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  uint extraout_ECX;
  uint uVar5;
  undefined4 unaff_EBX;
  char *pcVar6;
  undefined4 local_24;
  undefined *local_20;
  undefined4 local_1c;
  undefined1 local_14;
  
  local_24 = DAT_000cf4f4;
  local_20 = &DAT_00096330;
  local_14 = 0;
  local_1c = 0;
  DAT_000cf4f4 = &local_24;
  iVar2 = FUN_00075dc0();
  local_1c = 1;
  pcVar6 = (char *)(iVar2 + -0xbc);
  do {
    cVar1 = *param_2;
    *pcVar6 = cVar1;
    if (cVar1 == '\0') break;
    cVar1 = param_2[1];
    param_2 = param_2 + 2;
    pcVar6[1] = cVar1;
    pcVar6 = pcVar6 + 2;
  } while (cVar1 != '\0');
  FUN_00075e88();
  *(undefined4 *)(iVar2 + -0x1c) = unaff_EBX;
  *(uint *)(iVar2 + 0x98) = extraout_ECX;
  *(uint *)(iVar2 + -0x28) = (uint)((extraout_ECX & 4) != 0);
  *(uint *)(iVar2 + -0x20) = (uint)((extraout_ECX & 2) != 0);
  _DAT_000c4068 = (uint)((extraout_ECX & 0x10) != 0);
  DAT_000c406c = (uint)((extraout_ECX & 0x20) != 0);
  uVar3 = FUN_0005c320();
  *(undefined4 *)(iVar2 + -4) = uVar3;
  iVar4 = crt_malloc();
  *(int *)(iVar2 + -8) = iVar4;
  if (iVar4 == 0) {
    DAT_000cde50 = 2;
  }
  else {
    iVar4 = FUN_0007607b();
    *(int *)(iVar2 + -0x2c) = iVar4;
    if (iVar4 == 0) {
      DAT_000cde50 = 3;
    }
    else {
      FUN_0007612f();
      if (*(short *)(*(int *)(iVar2 + -4) + 0x52) == 0x1973) {
        DAT_000c41c0 = crt_malloc();
        if (DAT_000c41c0 == 0) {
          DAT_000cde50 = 2;
        }
        else {
          FUN_000749a0();
          if ((*(byte *)(iVar2 + -0x1b) & 0x80) != 0) {
            FUN_0001b280();
          }
          FUN_00018ef0();
          if (DAT_000c4050 < 8) {
            if (DAT_000c4050 == 4) {
              DAT_000cde40 = DAT_000c4050;
            }
          }
          else if (DAT_000c4050 < 9) {
            DAT_000cde40 = 3;
          }
          else if (0xf < DAT_000c4050) {
            if (DAT_000c4050 < 0x11) {
              DAT_000cde40 = 2;
            }
            else if (DAT_000c4050 == 0x20) {
              DAT_000cde40 = 1;
            }
          }
          uVar5 = *(uint *)(iVar2 + -0x1c) & 0xffff7fff;
          DAT_000c41bc = &PTR_FUN_000a0000;
          DAT_000cde2c = 0;
          DAT_000cde3c = (uint)*(ushort *)(&DAT_000a897e + uVar5 * 10);
          _DAT_000cde38 = *(undefined4 *)(&DAT_000a8982 + uVar5 * 10);
          DAT_000cde48 = 0;
          _DAT_000cde4a = 0;
          DAT_000cde24 = ((*(byte *)(*(int *)(iVar2 + -4) + 0x56) & 1) != 0) + 1;
          iVar4 = *(int *)(iVar2 + -4);
          *(undefined2 *)(iVar2 + 0x90) = 0;
          DAT_000c41d0._0_1_ = *(undefined1 *)(iVar4 + 0x5d);
          DAT_000c41c4 = (uint)*(ushort *)(iVar4 + 0x59);
          DAT_000c41c8 = (uint)*(ushort *)(iVar4 + 0x5b);
          _DAT_000c41cc = 0;
          DAT_000c41d2 = 0;
          FUN_000749a0();
          FUN_0001b7a0();
          _DAT_000cde44 = 0;
          if (*(int *)(iVar2 + -0x20) == 0) {
            iVar4 = FUN_00019a60();
            if (iVar4 == 0) {
              *(undefined4 *)(iVar2 + -0x20) = 1;
            }
          }
        }
      }
      else {
        DAT_000cde50 = 10;
      }
    }
  }
  DAT_000cf4f4 = (undefined4 *)local_24;
  return (char *)(iVar2 + -0xbc);
}

