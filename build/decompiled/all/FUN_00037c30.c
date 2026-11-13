
int FUN_00037c30(void)

{
  undefined4 *puVar1;
  
  FUN_00053ff0();
  puVar1 = (undefined4 *)FUN_00044fe0();
  puVar1[3] = 0;
  puVar1[4] = 0;
  puVar1[5] = &PTR_LAB_00098278;
  puVar1[7] = 0;
  puVar1[8] = &PTR_LAB_00098278;
  puVar1[6] = 0;
  puVar1[0x12] = 0x498;
  puVar1[0x13] = 300;
  *(undefined ***)((int)puVar1 + -0xe6) = &PTR_LAB_0009eff0;
  *puVar1 = &PTR_LAB_0009efcc;
  *(undefined ***)((int)puVar1 + -0xe2) = &PTR_LAB_0009efc4;
  return (int)puVar1 + -0xee;
}

