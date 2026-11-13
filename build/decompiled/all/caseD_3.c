
void switchD_0007d953::caseD_3(void)

{
  undefined2 *unaff_EBP;
  
  *unaff_EBP = 0x33f;
  unaff_EBP[2] = 0;
  unaff_EBP[4] = 0xffff;
  *(code **)(unaff_EBP + 0x3b) = FUN_0007c794;
  FUN_0007c794();
  return;
}

