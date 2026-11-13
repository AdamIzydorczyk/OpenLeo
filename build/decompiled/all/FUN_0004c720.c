
void __regparm3 FUN_0004c720(undefined4 param_1)

{
  int *unaff_EBX;
  char local_14 [4];
  
  *unaff_EBX = 0;
  do {
    *unaff_EBX = *unaff_EBX + 1;
    FUN_00057d30();
    FUN_00049920(param_1,&DAT_00091d77,local_14);
  } while (local_14[0] == ',');
  return;
}

