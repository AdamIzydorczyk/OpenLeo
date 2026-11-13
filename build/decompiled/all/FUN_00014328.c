
void __regparm3 FUN_00014328(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  byte bVar1;
  undefined1 uVar2;
  undefined2 uVar3;
  byte *pbVar4;
  undefined2 *puVar5;
  undefined4 extraout_EDX;
  
  if (DAT_000afb74[0xf] != 0) {
    FUN_0001520c(param_2,param_3);
    while( true ) {
      bVar1 = FUN_00015230();
      if (bVar1 == 0) break;
      if (bVar1 == 0) {
LAB_00014537:
        FUN_00015254();
      }
      else if (bVar1 < 2) {
        uVar2 = FUN_00015230();
        if (CONCAT31((int3)((uint)extraout_EDX >> 8),uVar2) == 0x60) {
          *(undefined1 *)(DAT_000afb74 + 10) = 0;
          if ((*DAT_000afb74 != 0) && ((*(byte *)(*DAT_000afb74 + 0x61) & 1) == 0)) {
            *(undefined1 *)((int)DAT_000afb74 + 0x43) = 0;
          }
        }
        else {
          *(undefined1 *)((int)DAT_000afb74 + 0x37) = uVar2;
          uVar3 = FUN_000138a8();
          *(undefined2 *)((int)DAT_000afb74 + 0x46) = uVar3;
          *(undefined2 *)(DAT_000afb74 + 0x11) = uVar3;
          *(undefined1 *)((int)DAT_000afb74 + 0x29) = 1;
          *(undefined4 *)((int)DAT_000afb74 + 0x2d) = 0;
          if ((*(byte *)((int)DAT_000afb74 + 0x52) & 0x80) == 0) {
            *(undefined1 *)((int)DAT_000afb74 + 0x56) = 0;
          }
          if ((*(byte *)((int)DAT_000afb74 + 0x52) & 8) == 0) {
            *(undefined1 *)((int)DAT_000afb74 + 0x53) = 0;
          }
        }
      }
      else {
        if (bVar1 != 2) goto LAB_00014537;
        bVar1 = FUN_00015230();
        if ((uint)bVar1 < (uint)*(ushort *)(DAT_000aefe4 + 9)) {
          *(byte *)((int)DAT_000afb74 + 0x2a) = bVar1;
          pbVar4 = (byte *)(*(int *)(DAT_000aefe4 + 0x13a) + (uint)bVar1 * 0xd9);
          *DAT_000afb74 = (int)pbVar4;
          if (pbVar4[*(byte *)((int)DAT_000afb74 + 0x37) + 1] < *pbVar4) {
            puVar5 = (undefined2 *)
                     (*(int *)(pbVar4 + 0xd5) +
                     (uint)pbVar4[*(byte *)((int)DAT_000afb74 + 0x37) + 1] * 0x1d);
            DAT_000afb74[1] = (int)puVar5;
            if ((*(byte *)((int)puVar5 + 0x11) & 0x40) == 0) {
              *(undefined1 *)((int)DAT_000afb74 + 0x31) =
                   *(undefined1 *)(DAT_000aefe4 + (uint)DAT_000afb68 + 0x10d);
            }
            else {
              *(undefined1 *)((int)DAT_000afb74 + 0x31) = *(undefined1 *)(puVar5 + 2);
            }
            *(undefined1 *)((int)DAT_000afb74 + 0x36) = *(undefined1 *)(puVar5 + 1);
            *(undefined2 *)((int)DAT_000afb74 + 0x2b) = *(undefined2 *)((int)puVar5 + 0x1b);
            uVar2 = *(undefined1 *)((int)puVar5 + 3);
            *(undefined1 *)((int)DAT_000afb74 + 0x43) = uVar2;
            *(undefined1 *)((int)DAT_000afb74 + 0x33) = uVar2;
            *(undefined2 *)((int)DAT_000afb74 + 0x41) = *puVar5;
            *(undefined1 *)(DAT_000afb74 + 0x10) = 0;
            *(undefined1 *)(DAT_000afb74 + 0x13) = 0;
            uVar3 = FUN_000138a8();
            *(undefined2 *)((int)DAT_000afb74 + 0x46) = uVar3;
            *(undefined2 *)(DAT_000afb74 + 0x11) = uVar3;
          }
        }
      }
    }
  }
  return;
}

