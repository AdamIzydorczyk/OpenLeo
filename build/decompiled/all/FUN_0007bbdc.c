
undefined8 __regparm3 FUN_0007bbdc(undefined4 param_1)

{
  code *pcVar1;
  undefined4 unaff_EBX;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return CONCAT44(unaff_EBX,param_1);
}

