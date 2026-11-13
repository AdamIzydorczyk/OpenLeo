
void __regparm3 FUN_000150e8(byte *param_1)

{
  int iVar1;
  int iVar2;
  
  DAT_000aefe8 = 0;
  DAT_000aefea = 0;
  DAT_000afb64 = 0;
  DAT_000aefec = (ushort)param_1[0xb];
  DAT_000aeff0 = 0;
  DAT_000aeff1 = 0;
  DAT_000afb60 = param_1[0xc];
  DAT_000aeff8 = 0;
  DAT_000afb62 = 0;
  DAT_000aeff4 = 2;
  DAT_000aeffc = 2;
  DAT_000aefee = 0;
  DAT_000a7331 = 0x40;
  iVar1 = 0;
  DAT_000aefe4 = param_1;
  DAT_000afb66 = DAT_000aefec;
  for (iVar2 = 0; iVar2 < (int)(uint)*DAT_000aefe4; iVar2 = iVar2 + 1) {
    *(undefined4 *)((int)&DAT_000af000 + iVar1) = 0;
    *(undefined4 *)((int)&DAT_000af004 + iVar1) = 0;
    *(undefined4 *)((int)&DAT_000af03c + iVar1) = 0;
    (&DAT_000af028)[iVar1] = 0;
    *(undefined1 *)((int)&DAT_000af029 + iVar1) = 0;
    (&DAT_000af033)[iVar1] = 0;
    (&DAT_000af043)[iVar1] = 0;
    (&DAT_000af040)[iVar1] = 0;
    (&DAT_000af052)[iVar1] = 0;
    (&DAT_000af051)[iVar1] = 0;
    *(undefined2 *)((int)&DAT_000af059 + iVar1) = 0;
    (&DAT_000af053)[iVar1] = 0;
    (&DAT_000af055)[iVar1] = 0;
    (&DAT_000af056)[iVar1] = 0;
    (&DAT_000af058)[iVar1] = 0;
    iVar1 = iVar1 + 0x5b;
  }
  return;
}

