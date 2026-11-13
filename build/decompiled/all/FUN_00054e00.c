
void __regparm3 FUN_00054e00(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 *extraout_EDX;
  
  FUN_00052c70(param_2,param_3);
  *(undefined4 *)((int)extraout_EDX + 0xd6) = *extraout_EDX;
  *(undefined4 *)((int)extraout_EDX + 0xda) = extraout_EDX[1];
  *(undefined4 *)((int)extraout_EDX + 0xe2) = 0;
  *(undefined4 *)((int)extraout_EDX + 0xa5) = 0;
  *(undefined4 *)((int)extraout_EDX + 0xba) = 0;
  *(undefined4 *)((int)extraout_EDX + 0xbe) = 0;
  *(undefined4 *)((int)extraout_EDX + 0xc2) = 0;
  *(undefined4 *)((int)extraout_EDX + 0xc6) = 0;
  *(undefined4 *)((int)extraout_EDX + 0xca) = 0;
  *(undefined4 *)((int)extraout_EDX + 0xce) = 0;
  *(undefined4 *)((int)extraout_EDX + 0xa9) = 0;
  *(undefined4 *)((int)extraout_EDX + 0xad) = 100;
  *(undefined4 *)((int)extraout_EDX + 0xb1) = 0x16;
  *(undefined4 *)((int)extraout_EDX + 0xd2) = 0;
  *(undefined4 *)((int)extraout_EDX + 0xe6) = 0;
  *(byte *)((int)extraout_EDX + 0xb6) = *(byte *)((int)extraout_EDX + 0xb6) & 0xc0;
  return;
}

