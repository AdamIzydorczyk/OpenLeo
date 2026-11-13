
void FUN_00050e60(void)

{
  undefined4 in_ECX;
  int extraout_ECX;
  int unaff_EBX;
  
  FUN_00044910(in_ECX);
  (&DAT_000cef5c)[unaff_EBX] = 1;
  (&DAT_000cef8c)[unaff_EBX] = 1;
  if (extraout_ECX != 0) {
    (*(code *)**(undefined4 **)(extraout_ECX + 8))();
  }
  return;
}

