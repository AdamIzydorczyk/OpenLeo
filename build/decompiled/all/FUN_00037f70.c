
undefined8 __regparm3 FUN_00037f70(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 *puVar3;
  
  iVar2 = FUN_00052860(param_2,param_3);
  *(undefined4 *)(iVar2 + 0x3c) = 0;
  *(undefined4 *)(iVar2 + 0x40) = 0;
  *(undefined ***)(iVar2 + 8) = &PTR_FUN_000981e8;
  uVar1 = *(undefined4 *)(iVar2 + 0x54);
  *(undefined ***)(iVar2 + 0xc) = &PTR_LAB_000981e0;
  puVar3 = (undefined4 *)FUN_00045070();
  puVar3[-0xf] = &PTR_LAB_0009ef38;
  *puVar3 = &PTR_LAB_0009ef14;
  puVar3[-0xe] = &PTR_LAB_0009ef0c;
  return CONCAT44(uVar1,puVar3 + -0x11);
}

