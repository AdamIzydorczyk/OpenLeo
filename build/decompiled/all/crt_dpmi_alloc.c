
/* INT31 AX=0100 linear block alloc; compares [0xa9bd4] */

undefined8 __regparm3 crt_dpmi_alloc(uint param_1)

{
  code *pcVar1;
  uint uVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  undefined1 *puVar5;
  undefined4 *puVar6;
  undefined1 *puVar7;
  byte bVar8;
  undefined4 local_20;
  undefined1 auStack_18 [4];
  
  puVar5 = auStack_18 + -4;
  puVar6 = &local_20;
  local_20 = param_1;
  if (DAT_000a9bd4 <= param_1) {
    if (DAT_000a9bee == '\0') {
      pcVar1 = (code *)swi(0x21);
      uVar2 = (*pcVar1)();
      bVar8 = 0;
      if ((uVar2 & 1) != 0) {
        pcVar1 = (code *)swi(0x21);
        (*pcVar1)();
        puVar5 = auStack_18;
        puVar6 = (undefined4 *)auStack_18;
        if ((bVar8 & 1) != 0) goto LAB_0007c151;
      }
      bVar8 = false;
      puVar3 = (undefined4 *)puVar5;
    }
    else {
      bVar8 = DAT_000a9bee == '\0';
      puVar3 = &local_20;
      if ((DAT_000a9bee == '\x01') &&
         (bVar8 = DAT_000a9bef == '\0', puVar3 = &local_20, DAT_000a9bef == '\x01')) {
        bVar8 = false;
        puVar3 = &local_20;
      }
    }
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    puVar6 = (undefined4 *)((int)puVar3 + 4);
    puVar7 = (undefined1 *)((int)puVar3 + 4);
    if ((bVar8 & 1) == 0) {
      uVar4 = DAT_000a9bc0;
      DAT_000a9bc0 = *(undefined4 *)((int)puVar3 + 4);
      goto LAB_0007c1a6;
    }
  }
LAB_0007c151:
  *(undefined4 *)((int)puVar6 + -4) = 0x7c156;
  puVar3 = (undefined4 *)FUN_0007b9a4();
  *puVar3 = 5;
  uVar4 = 0xffffffff;
  puVar7 = (undefined1 *)puVar6;
LAB_0007c1a6:
  return CONCAT44(*(undefined4 *)(puVar7 + 0x14),uVar4);
}

