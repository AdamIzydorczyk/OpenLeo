
undefined4 __regparm3 FUN_000769c3(undefined4 param_1,undefined4 param_2)

{
  code *pcVar1;
  undefined4 uVar2;
  undefined4 extraout_EDX;
  
  (*(code *)PTR_FUN_000a9b6c)(param_2);
  (*(code *)PTR_FUN_000a9b70)();
  FUN_00078b55(extraout_EDX);
  pcVar1 = (code *)swi(0x21);
  uVar2 = (*pcVar1)();
  return uVar2;
}

