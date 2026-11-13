
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 __regparm2
FUN_0001d014(undefined4 param_1,undefined4 param_2,uint *param_3,int param_4,int param_5,
            uint *param_6,int param_7,int param_8)

{
  uint3 uVar1;
  ushort uVar2;
  undefined2 uVar3;
  uint uVar4;
  uint uVar5;
  uint *puVar6;
  
  while (uVar5 = 0, param_7 != 0) {
    uVar5 = *param_6;
    uVar2 = (ushort)uVar5;
    if (0x10 < (byte)uVar5) {
      param_7 = param_7 + -1;
      uVar2 = CONCAT11((char)(uVar5 >> 8),(byte)uVar5 - 0x11);
    }
    param_6 = (uint *)((int)param_6 + *(int *)(&DAT_000a8e7c + (uint)(uVar2 & 0xff) * 4));
    if ((char)uVar2 == '\x0f') {
      param_6 = param_6 + (uVar2 >> 8);
    }
  }
  param_4 = param_4 - (int)param_3;
  _DAT_000a8df0 = param_5 + -400;
  uVar4 = *param_6;
  puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
  switch(uVar4 & 0xff) {
  case 1:
    goto switchD_0001d064_caseD_1;
  case 2:
    goto switchD_0001d064_caseD_2;
  case 3:
    goto switchD_0001d064_caseD_3;
  case 4:
    goto switchD_0001d064_caseD_4;
  case 5:
    goto switchD_0001d064_caseD_5;
  case 6:
    goto switchD_0001d064_caseD_6;
  case 7:
    goto switchD_0001d064_caseD_7;
  case 8:
    goto switchD_0001d064_caseD_8;
  case 9:
    goto switchD_0001d064_caseD_9;
  case 10:
    goto switchD_0001d064_caseD_a;
  case 0xb:
    goto switchD_0001d064_caseD_b;
  case 0xc:
    goto switchD_0001d064_caseD_c;
  case 0xd:
    goto switchD_0001d064_caseD_d;
  case 0xe:
    goto switchD_0001d064_caseD_e;
  case 0xf:
    goto switchD_0001d064_caseD_f;
  case 0x10:
    goto switchD_0001d064_caseD_10;
  case 0x11:
    goto switchD_0001d064_caseD_11;
  case 0x12:
    goto switchD_0001d064_caseD_12;
  case 0x13:
    goto switchD_0001d064_caseD_13;
  case 0x14:
    goto switchD_0001d064_caseD_14;
  case 0x15:
    goto switchD_0001d064_caseD_15;
  case 0x16:
    goto switchD_0001d064_caseD_16;
  case 0x17:
    goto switchD_0001d064_caseD_17;
  case 0x18:
    goto switchD_0001d064_caseD_18;
  case 0x19:
    goto switchD_0001d064_caseD_19;
  case 0x1a:
    goto switchD_0001d064_caseD_1a;
  case 0x1b:
    goto switchD_0001d064_caseD_1b;
  case 0x1c:
    goto switchD_0001d064_caseD_1c;
  case 0x1d:
    goto switchD_0001d064_caseD_1d;
  case 0x1e:
    goto switchD_0001d064_caseD_1e;
  case 0x1f:
    goto switchD_0001d064_caseD_1f;
  case 0x20:
    goto switchD_0001d064_caseD_20;
  case 0x21:
    goto switchD_0001d064_caseD_21;
  }
switchD_0001d064_caseD_0:
  do {
    param_6 = (uint *)((int)param_6 + 2);
    puVar6 = (uint *)((int)param_3 + param_4);
    for (uVar4 = uVar4 >> 8 & 0xff; uVar4 != 0; uVar4 = uVar4 - 1) {
      *param_3 = *puVar6;
      puVar6 = puVar6 + 1;
      param_3 = param_3 + 1;
    }
    uVar4 = *param_6;
    puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
    switch(uVar4 & 0xff) {
    case 1:
switchD_0001d064_caseD_1:
      do {
        uVar5 = *(uint *)((int)puVar6 + param_4);
        param_6 = (uint *)((int)param_6 + 2);
        *puVar6 = uVar5;
        param_3 = puVar6 + 1;
        *(char *)((int)puVar6 + 3) = (char)(uVar4 >> 8);
        uVar4 = *param_6;
        puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
        switch(uVar4 & 0xff) {
        case 0:
          goto switchD_0001d064_caseD_0;
        case 2:
switchD_0001d064_caseD_2:
          do {
            uVar5 = *(uint *)((int)puVar6 + param_4);
            param_6 = (uint *)((int)param_6 + 2);
            *puVar6 = uVar5;
            param_3 = puVar6 + 1;
            *(char *)((int)puVar6 + 2) = (char)(uVar4 >> 8);
            uVar4 = *param_6;
            puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
            switch(uVar4 & 0xff) {
            case 0:
              goto switchD_0001d064_caseD_0;
            case 1:
              goto switchD_0001d064_caseD_1;
            case 3:
switchD_0001d064_caseD_3:
              do {
                param_6 = param_6 + 1;
                *param_3 = CONCAT22((short)(uVar4 >> 0x10),*(undefined2 *)((int)param_3 + param_4));
                param_3 = param_3 + 1;
                uVar4 = *param_6;
                puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                switch(uVar4 & 0xff) {
                case 0:
                  goto switchD_0001d064_caseD_0;
                case 1:
                  goto switchD_0001d064_caseD_1;
                case 2:
                  goto switchD_0001d064_caseD_2;
                case 4:
switchD_0001d064_caseD_4:
                  do {
                    uVar5 = CONCAT22((short)((uint)*(undefined4 *)((int)param_3 + param_4) >> 0x10),
                                     CONCAT11((char)(uVar4 >> 8),
                                              (char)*(undefined4 *)((int)param_3 + param_4)));
                    param_6 = (uint *)((int)param_6 + 2);
                    *param_3 = uVar5;
                    param_3 = param_3 + 1;
                    uVar4 = *param_6;
                    puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                    switch(uVar4 & 0xff) {
                    case 0:
                      goto switchD_0001d064_caseD_0;
                    case 1:
                      goto switchD_0001d064_caseD_1;
                    case 2:
                      goto switchD_0001d064_caseD_2;
                    case 3:
                      goto switchD_0001d064_caseD_3;
                    case 5:
switchD_0001d064_caseD_5:
                      do {
                        uVar1 = (uint3)(uVar4 >> 8);
                        param_6 = param_6 + 1;
                        *param_3 = CONCAT31(uVar1 >> 0x10,
                                            *(undefined1 *)((int)param_3 + param_4 + 2)) << 0x10 |
                                   CONCAT31(uVar1,*(undefined1 *)((int)param_3 + param_4)) & 0xffff;
                        param_3 = param_3 + 1;
                        uVar4 = *param_6;
                        puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                        switch(uVar4 & 0xff) {
                        case 0:
                          goto switchD_0001d064_caseD_0;
                        case 1:
                          goto switchD_0001d064_caseD_1;
                        case 2:
                          goto switchD_0001d064_caseD_2;
                        case 3:
                          goto switchD_0001d064_caseD_3;
                        case 4:
                          goto switchD_0001d064_caseD_4;
                        case 6:
switchD_0001d064_caseD_6:
                          do {
                            param_6 = param_6 + 1;
                            *param_3 = CONCAT31((int3)(uVar4 >> 8),
                                                *(undefined1 *)((int)param_3 + param_4)) & 0xffffff
                                       | (uint)*(byte *)((int)param_3 + param_4 + 3) << 0x18;
                            param_3 = param_3 + 1;
                            uVar4 = *param_6;
                            puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                            switch(uVar4 & 0xff) {
                            case 0:
                              goto switchD_0001d064_caseD_0;
                            case 1:
                              goto switchD_0001d064_caseD_1;
                            case 2:
                              goto switchD_0001d064_caseD_2;
                            case 3:
                              goto switchD_0001d064_caseD_3;
                            case 4:
                              goto switchD_0001d064_caseD_4;
                            case 5:
                              goto switchD_0001d064_caseD_5;
                            case 7:
switchD_0001d064_caseD_7:
                              do {
                                param_6 = param_6 + 1;
                                *param_3 = CONCAT31((int3)(uVar4 >> 8),
                                                    *(undefined1 *)((int)param_3 + param_4));
                                param_3 = param_3 + 1;
                                uVar4 = *param_6;
                                puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                switch(uVar4 & 0xff) {
                                case 0:
                                  goto switchD_0001d064_caseD_0;
                                case 1:
                                  goto switchD_0001d064_caseD_1;
                                case 2:
                                  goto switchD_0001d064_caseD_2;
                                case 3:
                                  goto switchD_0001d064_caseD_3;
                                case 4:
                                  goto switchD_0001d064_caseD_4;
                                case 5:
                                  goto switchD_0001d064_caseD_5;
                                case 6:
                                  goto switchD_0001d064_caseD_6;
                                case 8:
switchD_0001d064_caseD_8:
                                  do {
                                    uVar5 = CONCAT31((int3)((uint)*(undefined4 *)
                                                                   ((int)param_3 + param_4) >> 8),
                                                     (char)(uVar4 >> 8));
                                    param_6 = (uint *)((int)param_6 + 2);
                                    *param_3 = uVar5;
                                    param_3 = param_3 + 1;
                                    uVar4 = *param_6;
                                    puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                    switch(uVar4 & 0xff) {
                                    case 0:
                                      goto switchD_0001d064_caseD_0;
                                    case 1:
                                      goto switchD_0001d064_caseD_1;
                                    case 2:
                                      goto switchD_0001d064_caseD_2;
                                    case 3:
                                      goto switchD_0001d064_caseD_3;
                                    case 4:
                                      goto switchD_0001d064_caseD_4;
                                    case 5:
                                      goto switchD_0001d064_caseD_5;
                                    case 6:
                                      goto switchD_0001d064_caseD_6;
                                    case 7:
                                      goto switchD_0001d064_caseD_7;
                                    case 9:
switchD_0001d064_caseD_9:
                                      do {
                                        uVar1 = (uint3)(uVar5 >> 8);
                                        uVar5 = (uint)uVar1 << 8;
                                        param_6 = param_6 + 1;
                                        *param_3 = CONCAT22((short)(uVar4 >> 0x10),
                                                            *(undefined2 *)
                                                             ((int)param_3 + param_4 + 1)) << 8 |
                                                   (uint)(uVar1 >> 0x10);
                                        param_3 = param_3 + 1;
                                        uVar4 = *param_6;
                                        puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                        switch(uVar4 & 0xff) {
                                        case 0:
                                          goto switchD_0001d064_caseD_0;
                                        case 1:
                                          goto switchD_0001d064_caseD_1;
                                        case 2:
                                          goto switchD_0001d064_caseD_2;
                                        case 3:
                                          goto switchD_0001d064_caseD_3;
                                        case 4:
                                          goto switchD_0001d064_caseD_4;
                                        case 5:
                                          goto switchD_0001d064_caseD_5;
                                        case 6:
                                          goto switchD_0001d064_caseD_6;
                                        case 7:
                                          goto switchD_0001d064_caseD_7;
                                        case 8:
                                          goto switchD_0001d064_caseD_8;
                                        case 10:
switchD_0001d064_caseD_a:
                                          do {
                                            param_6 = param_6 + 1;
                                            *param_3 = CONCAT22((ushort)(byte)(uVar4 >> 0x18) |
                                                                (ushort)(((uint)*(byte *)((int)
                                                  param_3 + param_4 + 3) << 0x18) >> 0x10),
                                                  CONCAT11(*(undefined1 *)
                                                            ((int)param_3 + param_4 + 1),
                                                           (char)(uVar4 >> 8)));
                                            param_3 = param_3 + 1;
                                            uVar4 = *param_6;
                                            puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                            switch(uVar4 & 0xff) {
                                            case 0:
                                              goto switchD_0001d064_caseD_0;
                                            case 1:
                                              goto switchD_0001d064_caseD_1;
                                            case 2:
                                              goto switchD_0001d064_caseD_2;
                                            case 3:
                                              goto switchD_0001d064_caseD_3;
                                            case 4:
                                              goto switchD_0001d064_caseD_4;
                                            case 5:
                                              goto switchD_0001d064_caseD_5;
                                            case 6:
                                              goto switchD_0001d064_caseD_6;
                                            case 7:
                                              goto switchD_0001d064_caseD_7;
                                            case 8:
                                              goto switchD_0001d064_caseD_8;
                                            case 9:
                                              goto switchD_0001d064_caseD_9;
                                            case 0xb:
switchD_0001d064_caseD_b:
                                              do {
                                                param_6 = param_6 + 1;
                                                *param_3 = CONCAT22((short)(uVar4 >> 0x10),
                                                                    CONCAT11(*(undefined1 *)
                                                                              ((int)param_3 +
                                                                              param_4 + 1),
                                                                             (char)(uVar4 >> 8)));
                                                param_3 = param_3 + 1;
                                                uVar4 = *param_6;
                                                puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                switch(uVar4 & 0xff) {
                                                case 0:
                                                  goto switchD_0001d064_caseD_0;
                                                case 1:
                                                  goto switchD_0001d064_caseD_1;
                                                case 2:
                                                  goto switchD_0001d064_caseD_2;
                                                case 3:
                                                  goto switchD_0001d064_caseD_3;
                                                case 4:
                                                  goto switchD_0001d064_caseD_4;
                                                case 5:
                                                  goto switchD_0001d064_caseD_5;
                                                case 6:
                                                  goto switchD_0001d064_caseD_6;
                                                case 7:
                                                  goto switchD_0001d064_caseD_7;
                                                case 8:
                                                  goto switchD_0001d064_caseD_8;
                                                case 9:
                                                  goto switchD_0001d064_caseD_9;
                                                case 10:
                                                  goto switchD_0001d064_caseD_a;
                                                case 0xc:
switchD_0001d064_caseD_c:
                                                  do {
                                                    uVar2 = (ushort)(uVar4 >> 0x10);
                                                    param_6 = param_6 + 1;
                                                    *param_3 = (uint)uVar2 |
                                                               CONCAT22(uVar2,*(undefined2 *)
                                                                               ((int)param_3 +
                                                                               param_4 + 2)) << 0x10
                                                    ;
                                                    param_3 = param_3 + 1;
                                                    uVar4 = *param_6;
                                                    puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                    switch(uVar4 & 0xff) {
                                                    case 0:
                                                      goto switchD_0001d064_caseD_0;
                                                    case 1:
                                                      goto switchD_0001d064_caseD_1;
                                                    case 2:
                                                      goto switchD_0001d064_caseD_2;
                                                    case 3:
                                                      goto switchD_0001d064_caseD_3;
                                                    case 4:
                                                      goto switchD_0001d064_caseD_4;
                                                    case 5:
                                                      goto switchD_0001d064_caseD_5;
                                                    case 6:
                                                      goto switchD_0001d064_caseD_6;
                                                    case 7:
                                                      goto switchD_0001d064_caseD_7;
                                                    case 8:
                                                      goto switchD_0001d064_caseD_8;
                                                    case 9:
                                                      goto switchD_0001d064_caseD_9;
                                                    case 10:
                                                      goto switchD_0001d064_caseD_a;
                                                    case 0xb:
                                                      goto switchD_0001d064_caseD_b;
                                                    case 0xd:
switchD_0001d064_caseD_d:
                                                      do {
                                                        uVar1 = (uint3)(uVar4 >> 8);
                                                        param_6 = param_6 + 1;
                                                        *param_3 = CONCAT31(uVar1,*(undefined1 *)
                                                                                   ((int)param_3 +
                                                                                   param_4 + 2)) <<
                                                                   0x10 | (uint)(uVar1 >> 8);
                                                        param_3 = param_3 + 1;
                                                        uVar4 = *param_6;
                                                        puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                        switch(uVar4 & 0xff) {
                                                        case 0:
                                                          goto switchD_0001d064_caseD_0;
                                                        case 1:
                                                          goto switchD_0001d064_caseD_1;
                                                        case 2:
                                                          goto switchD_0001d064_caseD_2;
                                                        case 3:
                                                          goto switchD_0001d064_caseD_3;
                                                        case 4:
                                                          goto switchD_0001d064_caseD_4;
                                                        case 5:
                                                          goto switchD_0001d064_caseD_5;
                                                        case 6:
                                                          goto switchD_0001d064_caseD_6;
                                                        case 7:
                                                          goto switchD_0001d064_caseD_7;
                                                        case 8:
                                                          goto switchD_0001d064_caseD_8;
                                                        case 9:
                                                          goto switchD_0001d064_caseD_9;
                                                        case 10:
                                                          goto switchD_0001d064_caseD_a;
                                                        case 0xb:
                                                          goto switchD_0001d064_caseD_b;
                                                        case 0xc:
                                                          goto switchD_0001d064_caseD_c;
                                                        case 0xe:
switchD_0001d064_caseD_e:
                                                          do {
                                                            uVar1 = (uint3)(uVar4 >> 8);
                                                            param_6 = param_6 + 1;
                                                            *param_3 = (uint)uVar1 |
                                                                       CONCAT31(uVar1,*(undefined1 *
                                                                                       )((int)
                                                  param_3 + param_4 + 3)) << 0x18;
                                                  param_3 = param_3 + 1;
                                                  uVar4 = *param_6;
                                                  puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                  switch(uVar4 & 0xff) {
                                                  case 0:
                                                    goto switchD_0001d064_caseD_0;
                                                  case 1:
                                                    goto switchD_0001d064_caseD_1;
                                                  case 2:
                                                    goto switchD_0001d064_caseD_2;
                                                  case 3:
                                                    goto switchD_0001d064_caseD_3;
                                                  case 4:
                                                    goto switchD_0001d064_caseD_4;
                                                  case 5:
                                                    goto switchD_0001d064_caseD_5;
                                                  case 6:
                                                    goto switchD_0001d064_caseD_6;
                                                  case 7:
                                                    goto switchD_0001d064_caseD_7;
                                                  case 8:
                                                    goto switchD_0001d064_caseD_8;
                                                  case 9:
                                                    goto switchD_0001d064_caseD_9;
                                                  case 10:
                                                    goto switchD_0001d064_caseD_a;
                                                  case 0xb:
                                                    goto switchD_0001d064_caseD_b;
                                                  case 0xc:
                                                    goto switchD_0001d064_caseD_c;
                                                  case 0xd:
                                                    goto switchD_0001d064_caseD_d;
                                                  case 0xf:
switchD_0001d064_caseD_f:
                                                    do {
                                                      param_6 = (uint *)((int)param_6 + 2);
                                                      for (uVar4 = uVar4 >> 8 & 0xff; uVar4 != 0;
                                                          uVar4 = uVar4 - 1) {
                                                        *param_3 = *param_6;
                                                        param_6 = param_6 + 1;
                                                        param_3 = param_3 + 1;
                                                      }
                                                      uVar4 = *param_6;
                                                      puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                      switch(uVar4 & 0xff) {
                                                      case 0:
                                                        goto switchD_0001d064_caseD_0;
                                                      case 1:
                                                        goto switchD_0001d064_caseD_1;
                                                      case 2:
                                                        goto switchD_0001d064_caseD_2;
                                                      case 3:
                                                        goto switchD_0001d064_caseD_3;
                                                      case 4:
                                                        goto switchD_0001d064_caseD_4;
                                                      case 5:
                                                        goto switchD_0001d064_caseD_5;
                                                      case 6:
                                                        goto switchD_0001d064_caseD_6;
                                                      case 7:
                                                        goto switchD_0001d064_caseD_7;
                                                      case 8:
                                                        goto switchD_0001d064_caseD_8;
                                                      case 9:
                                                        goto switchD_0001d064_caseD_9;
                                                      case 10:
                                                        goto switchD_0001d064_caseD_a;
                                                      case 0xb:
                                                        goto switchD_0001d064_caseD_b;
                                                      case 0xc:
                                                        goto switchD_0001d064_caseD_c;
                                                      case 0xd:
                                                        goto switchD_0001d064_caseD_d;
                                                      case 0xe:
                                                        goto switchD_0001d064_caseD_e;
                                                      case 0x10:
switchD_0001d064_caseD_10:
                                                        do {
                                                          uVar5 = uVar5 << 0x10 | uVar4 >> 0x10;
                                                          uVar3 = (undefined2)(uVar4 >> 0x10);
                                                          for (uVar4 = uVar4 >> 8 & 0xff; uVar4 != 0
                                                              ; uVar4 = uVar4 - 1) {
                                                            *param_3 = CONCAT22(uVar3,uVar3);
                                                            param_3 = param_3 + 1;
                                                          }
                                                          param_6 = param_6 + 1;
                                                          uVar4 = *param_6;
                                                          puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                          switch(uVar4 & 0xff) {
                                                          case 0:
                                                            goto switchD_0001d064_caseD_0;
                                                          case 1:
                                                            goto switchD_0001d064_caseD_1;
                                                          case 2:
                                                            goto switchD_0001d064_caseD_2;
                                                          case 3:
                                                            goto switchD_0001d064_caseD_3;
                                                          case 4:
                                                            goto switchD_0001d064_caseD_4;
                                                          case 5:
                                                            goto switchD_0001d064_caseD_5;
                                                          case 6:
                                                            goto switchD_0001d064_caseD_6;
                                                          case 7:
                                                            goto switchD_0001d064_caseD_7;
                                                          case 8:
                                                            goto switchD_0001d064_caseD_8;
                                                          case 9:
                                                            goto switchD_0001d064_caseD_9;
                                                          case 10:
                                                            goto switchD_0001d064_caseD_a;
                                                          case 0xb:
                                                            goto switchD_0001d064_caseD_b;
                                                          case 0xc:
                                                            goto switchD_0001d064_caseD_c;
                                                          case 0xd:
                                                            goto switchD_0001d064_caseD_d;
                                                          case 0xe:
                                                            goto switchD_0001d064_caseD_e;
                                                          case 0xf:
                                                            goto switchD_0001d064_caseD_f;
                                                          case 0x11:
switchD_0001d064_caseD_11:
                                                            do {
                                                              param_6 = (uint *)((int)param_6 + 2);
                                                              puVar6 = (uint *)((int)param_3 +
                                                                               param_4);
                                                              for (uVar4 = uVar4 >> 8 & 0xff;
                                                                  uVar4 != 0; uVar4 = uVar4 - 1) {
                                                                *param_3 = *puVar6;
                                                                puVar6 = puVar6 + 1;
                                                                param_3 = param_3 + 1;
                                                              }
                                                              param_8 = param_8 + -1;
                                                              if (param_8 == 0) {
LAB_0001d06b:
                                                                return CONCAT44(param_2,param_1);
                                                              }
                                                              param_3 = param_3 + 0x50;
                                                              param_4 = param_4 + _DAT_000a8df0;
                                                              uVar4 = *param_6;
                                                              puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                              switch(uVar4 & 0xff) {
                                                              case 0:
                                                                goto switchD_0001d064_caseD_0;
                                                              case 1:
                                                                goto switchD_0001d064_caseD_1;
                                                              case 2:
                                                                goto switchD_0001d064_caseD_2;
                                                              case 3:
                                                                goto switchD_0001d064_caseD_3;
                                                              case 4:
                                                                goto switchD_0001d064_caseD_4;
                                                              case 5:
                                                                goto switchD_0001d064_caseD_5;
                                                              case 6:
                                                                goto switchD_0001d064_caseD_6;
                                                              case 7:
                                                                goto switchD_0001d064_caseD_7;
                                                              case 8:
                                                                goto switchD_0001d064_caseD_8;
                                                              case 9:
                                                                goto switchD_0001d064_caseD_9;
                                                              case 10:
                                                                goto switchD_0001d064_caseD_a;
                                                              case 0xb:
                                                                goto switchD_0001d064_caseD_b;
                                                              case 0xc:
                                                                goto switchD_0001d064_caseD_c;
                                                              case 0xd:
                                                                goto switchD_0001d064_caseD_d;
                                                              case 0xe:
                                                                goto switchD_0001d064_caseD_e;
                                                              case 0xf:
                                                                goto switchD_0001d064_caseD_f;
                                                              case 0x10:
                                                                goto switchD_0001d064_caseD_10;
                                                              case 0x12:
switchD_0001d064_caseD_12:
                                                                do {
                                                                  uVar5 = *(uint *)((int)param_3 +
                                                                                   param_4);
                                                                  param_6 = (uint *)((int)param_6 +
                                                                                    2);
                                                                  *param_3 = uVar5;
                                                                  *(char *)((int)param_3 + 3) =
                                                                       (char)(uVar4 >> 8);
                                                                  param_8 = param_8 + -1;
                                                                  if (param_8 == 0)
                                                                  goto LAB_0001d06b;
                                                                  param_3 = param_3 + 0x51;
                                                                  param_4 = param_4 + _DAT_000a8df0;
                                                                  uVar4 = *param_6;
                                                                  puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                                  switch(uVar4 & 0xff) {
                                                                  case 0:
                                                                    goto switchD_0001d064_caseD_0;
                                                                  case 1:
                                                                    goto switchD_0001d064_caseD_1;
                                                                  case 2:
                                                                    goto switchD_0001d064_caseD_2;
                                                                  case 3:
                                                                    goto switchD_0001d064_caseD_3;
                                                                  case 4:
                                                                    goto switchD_0001d064_caseD_4;
                                                                  case 5:
                                                                    goto switchD_0001d064_caseD_5;
                                                                  case 6:
                                                                    goto switchD_0001d064_caseD_6;
                                                                  case 7:
                                                                    goto switchD_0001d064_caseD_7;
                                                                  case 8:
                                                                    goto switchD_0001d064_caseD_8;
                                                                  case 9:
                                                                    goto switchD_0001d064_caseD_9;
                                                                  case 10:
                                                                    goto switchD_0001d064_caseD_a;
                                                                  case 0xb:
                                                                    goto switchD_0001d064_caseD_b;
                                                                  case 0xc:
                                                                    goto switchD_0001d064_caseD_c;
                                                                  case 0xd:
                                                                    goto switchD_0001d064_caseD_d;
                                                                  case 0xe:
                                                                    goto switchD_0001d064_caseD_e;
                                                                  case 0xf:
                                                                    goto switchD_0001d064_caseD_f;
                                                                  case 0x10:
                                                                    goto switchD_0001d064_caseD_10;
                                                                  case 0x11:
                                                                    goto switchD_0001d064_caseD_11;
                                                                  case 0x13:
switchD_0001d064_caseD_13:
                                                                    do {
                                                                      uVar5 = *(uint *)((int)param_3
                                                                                       + param_4);
                                                                      param_6 = (uint *)((int)
                                                  param_6 + 2);
                                                  *param_3 = uVar5;
                                                  *(char *)((int)param_3 + 2) = (char)(uVar4 >> 8);
                                                  param_8 = param_8 + -1;
                                                  if (param_8 == 0) goto LAB_0001d06b;
                                                  param_3 = param_3 + 0x51;
                                                  param_4 = param_4 + _DAT_000a8df0;
                                                  uVar4 = *param_6;
                                                  puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                  switch(uVar4 & 0xff) {
                                                  case 0:
                                                    goto switchD_0001d064_caseD_0;
                                                  case 1:
                                                    goto switchD_0001d064_caseD_1;
                                                  case 2:
                                                    goto switchD_0001d064_caseD_2;
                                                  case 3:
                                                    goto switchD_0001d064_caseD_3;
                                                  case 4:
                                                    goto switchD_0001d064_caseD_4;
                                                  case 5:
                                                    goto switchD_0001d064_caseD_5;
                                                  case 6:
                                                    goto switchD_0001d064_caseD_6;
                                                  case 7:
                                                    goto switchD_0001d064_caseD_7;
                                                  case 8:
                                                    goto switchD_0001d064_caseD_8;
                                                  case 9:
                                                    goto switchD_0001d064_caseD_9;
                                                  case 10:
                                                    goto switchD_0001d064_caseD_a;
                                                  case 0xb:
                                                    goto switchD_0001d064_caseD_b;
                                                  case 0xc:
                                                    goto switchD_0001d064_caseD_c;
                                                  case 0xd:
                                                    goto switchD_0001d064_caseD_d;
                                                  case 0xe:
                                                    goto switchD_0001d064_caseD_e;
                                                  case 0xf:
                                                    goto switchD_0001d064_caseD_f;
                                                  case 0x10:
                                                    goto switchD_0001d064_caseD_10;
                                                  case 0x11:
                                                    goto switchD_0001d064_caseD_11;
                                                  case 0x12:
                                                    goto switchD_0001d064_caseD_12;
                                                  case 0x14:
switchD_0001d064_caseD_14:
                                                    do {
                                                      param_6 = param_6 + 1;
                                                      *param_3 = CONCAT22((short)(uVar4 >> 0x10),
                                                                          *(undefined2 *)
                                                                           ((int)param_3 + param_4))
                                                      ;
                                                      param_8 = param_8 + -1;
                                                      if (param_8 == 0) goto LAB_0001d06b;
                                                      param_3 = param_3 + 0x51;
                                                      param_4 = param_4 + _DAT_000a8df0;
                                                      uVar4 = *param_6;
                                                      puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                      switch(uVar4 & 0xff) {
                                                      case 0:
                                                        goto switchD_0001d064_caseD_0;
                                                      case 1:
                                                        goto switchD_0001d064_caseD_1;
                                                      case 2:
                                                        goto switchD_0001d064_caseD_2;
                                                      case 3:
                                                        goto switchD_0001d064_caseD_3;
                                                      case 4:
                                                        goto switchD_0001d064_caseD_4;
                                                      case 5:
                                                        goto switchD_0001d064_caseD_5;
                                                      case 6:
                                                        goto switchD_0001d064_caseD_6;
                                                      case 7:
                                                        goto switchD_0001d064_caseD_7;
                                                      case 8:
                                                        goto switchD_0001d064_caseD_8;
                                                      case 9:
                                                        goto switchD_0001d064_caseD_9;
                                                      case 10:
                                                        goto switchD_0001d064_caseD_a;
                                                      case 0xb:
                                                        goto switchD_0001d064_caseD_b;
                                                      case 0xc:
                                                        goto switchD_0001d064_caseD_c;
                                                      case 0xd:
                                                        goto switchD_0001d064_caseD_d;
                                                      case 0xe:
                                                        goto switchD_0001d064_caseD_e;
                                                      case 0xf:
                                                        goto switchD_0001d064_caseD_f;
                                                      case 0x10:
                                                        goto switchD_0001d064_caseD_10;
                                                      case 0x11:
                                                        goto switchD_0001d064_caseD_11;
                                                      case 0x12:
                                                        goto switchD_0001d064_caseD_12;
                                                      case 0x13:
                                                        goto switchD_0001d064_caseD_13;
                                                      case 0x15:
switchD_0001d064_caseD_15:
                                                        do {
                                                          uVar5 = CONCAT22((short)((uint)*(
                                                  undefined4 *)((int)param_3 + param_4) >> 0x10),
                                                  CONCAT11((char)(uVar4 >> 8),
                                                           (char)*(undefined4 *)
                                                                  ((int)param_3 + param_4)));
                                                  param_6 = (uint *)((int)param_6 + 2);
                                                  *param_3 = uVar5;
                                                  param_8 = param_8 + -1;
                                                  if (param_8 == 0) goto LAB_0001d06b;
                                                  param_3 = param_3 + 0x51;
                                                  param_4 = param_4 + _DAT_000a8df0;
                                                  uVar4 = *param_6;
                                                  puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                  switch(uVar4 & 0xff) {
                                                  case 0:
                                                    goto switchD_0001d064_caseD_0;
                                                  case 1:
                                                    goto switchD_0001d064_caseD_1;
                                                  case 2:
                                                    goto switchD_0001d064_caseD_2;
                                                  case 3:
                                                    goto switchD_0001d064_caseD_3;
                                                  case 4:
                                                    goto switchD_0001d064_caseD_4;
                                                  case 5:
                                                    goto switchD_0001d064_caseD_5;
                                                  case 6:
                                                    goto switchD_0001d064_caseD_6;
                                                  case 7:
                                                    goto switchD_0001d064_caseD_7;
                                                  case 8:
                                                    goto switchD_0001d064_caseD_8;
                                                  case 9:
                                                    goto switchD_0001d064_caseD_9;
                                                  case 10:
                                                    goto switchD_0001d064_caseD_a;
                                                  case 0xb:
                                                    goto switchD_0001d064_caseD_b;
                                                  case 0xc:
                                                    goto switchD_0001d064_caseD_c;
                                                  case 0xd:
                                                    goto switchD_0001d064_caseD_d;
                                                  case 0xe:
                                                    goto switchD_0001d064_caseD_e;
                                                  case 0xf:
                                                    goto switchD_0001d064_caseD_f;
                                                  case 0x10:
                                                    goto switchD_0001d064_caseD_10;
                                                  case 0x11:
                                                    goto switchD_0001d064_caseD_11;
                                                  case 0x12:
                                                    goto switchD_0001d064_caseD_12;
                                                  case 0x13:
                                                    goto switchD_0001d064_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d064_caseD_14;
                                                  case 0x16:
switchD_0001d064_caseD_16:
                                                    do {
                                                      uVar1 = (uint3)(uVar4 >> 8);
                                                      param_6 = param_6 + 1;
                                                      *param_3 = CONCAT31(uVar1 >> 0x10,
                                                                          *(undefined1 *)
                                                                           ((int)param_3 +
                                                                           param_4 + 2)) << 0x10 |
                                                                 CONCAT31(uVar1,*(undefined1 *)
                                                                                 ((int)param_3 +
                                                                                 param_4)) & 0xffff;
                                                      param_8 = param_8 + -1;
                                                      if (param_8 == 0) goto LAB_0001d06b;
                                                      param_3 = param_3 + 0x51;
                                                      param_4 = param_4 + _DAT_000a8df0;
                                                      uVar4 = *param_6;
                                                      puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                      switch(uVar4 & 0xff) {
                                                      case 0:
                                                        goto switchD_0001d064_caseD_0;
                                                      case 1:
                                                        goto switchD_0001d064_caseD_1;
                                                      case 2:
                                                        goto switchD_0001d064_caseD_2;
                                                      case 3:
                                                        goto switchD_0001d064_caseD_3;
                                                      case 4:
                                                        goto switchD_0001d064_caseD_4;
                                                      case 5:
                                                        goto switchD_0001d064_caseD_5;
                                                      case 6:
                                                        goto switchD_0001d064_caseD_6;
                                                      case 7:
                                                        goto switchD_0001d064_caseD_7;
                                                      case 8:
                                                        goto switchD_0001d064_caseD_8;
                                                      case 9:
                                                        goto switchD_0001d064_caseD_9;
                                                      case 10:
                                                        goto switchD_0001d064_caseD_a;
                                                      case 0xb:
                                                        goto switchD_0001d064_caseD_b;
                                                      case 0xc:
                                                        goto switchD_0001d064_caseD_c;
                                                      case 0xd:
                                                        goto switchD_0001d064_caseD_d;
                                                      case 0xe:
                                                        goto switchD_0001d064_caseD_e;
                                                      case 0xf:
                                                        goto switchD_0001d064_caseD_f;
                                                      case 0x10:
                                                        goto switchD_0001d064_caseD_10;
                                                      case 0x11:
                                                        goto switchD_0001d064_caseD_11;
                                                      case 0x12:
                                                        goto switchD_0001d064_caseD_12;
                                                      case 0x13:
                                                        goto switchD_0001d064_caseD_13;
                                                      case 0x14:
                                                        goto switchD_0001d064_caseD_14;
                                                      case 0x15:
                                                        goto switchD_0001d064_caseD_15;
                                                      case 0x17:
switchD_0001d064_caseD_17:
                                                        do {
                                                          param_6 = param_6 + 1;
                                                          *param_3 = CONCAT31((int3)(uVar4 >> 8),
                                                                              *(undefined1 *)
                                                                               ((int)param_3 +
                                                                               param_4)) & 0xffffff
                                                                     | (uint)*(byte *)((int)param_3
                                                                                      + param_4 + 3)
                                                                       << 0x18;
                                                          param_8 = param_8 + -1;
                                                          if (param_8 == 0) goto LAB_0001d06b;
                                                          param_3 = param_3 + 0x51;
                                                          param_4 = param_4 + _DAT_000a8df0;
                                                          uVar4 = *param_6;
                                                          puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                          switch(uVar4 & 0xff) {
                                                          case 0:
                                                            goto switchD_0001d064_caseD_0;
                                                          case 1:
                                                            goto switchD_0001d064_caseD_1;
                                                          case 2:
                                                            goto switchD_0001d064_caseD_2;
                                                          case 3:
                                                            goto switchD_0001d064_caseD_3;
                                                          case 4:
                                                            goto switchD_0001d064_caseD_4;
                                                          case 5:
                                                            goto switchD_0001d064_caseD_5;
                                                          case 6:
                                                            goto switchD_0001d064_caseD_6;
                                                          case 7:
                                                            goto switchD_0001d064_caseD_7;
                                                          case 8:
                                                            goto switchD_0001d064_caseD_8;
                                                          case 9:
                                                            goto switchD_0001d064_caseD_9;
                                                          case 10:
                                                            goto switchD_0001d064_caseD_a;
                                                          case 0xb:
                                                            goto switchD_0001d064_caseD_b;
                                                          case 0xc:
                                                            goto switchD_0001d064_caseD_c;
                                                          case 0xd:
                                                            goto switchD_0001d064_caseD_d;
                                                          case 0xe:
                                                            goto switchD_0001d064_caseD_e;
                                                          case 0xf:
                                                            goto switchD_0001d064_caseD_f;
                                                          case 0x10:
                                                            goto switchD_0001d064_caseD_10;
                                                          case 0x11:
                                                            goto switchD_0001d064_caseD_11;
                                                          case 0x12:
                                                            goto switchD_0001d064_caseD_12;
                                                          case 0x13:
                                                            goto switchD_0001d064_caseD_13;
                                                          case 0x14:
                                                            goto switchD_0001d064_caseD_14;
                                                          case 0x15:
                                                            goto switchD_0001d064_caseD_15;
                                                          case 0x16:
                                                            goto switchD_0001d064_caseD_16;
                                                          case 0x18:
switchD_0001d064_caseD_18:
                                                            do {
                                                              param_6 = param_6 + 1;
                                                              *param_3 = CONCAT31((int3)(uVar4 >> 8)
                                                                                  ,*(undefined1 *)
                                                                                    ((int)param_3 +
                                                                                    param_4));
                                                              param_8 = param_8 + -1;
                                                              if (param_8 == 0) goto LAB_0001d06b;
                                                              param_3 = param_3 + 0x51;
                                                              param_4 = param_4 + _DAT_000a8df0;
                                                              uVar4 = *param_6;
                                                              puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                              switch(uVar4 & 0xff) {
                                                              case 0:
                                                                goto switchD_0001d064_caseD_0;
                                                              case 1:
                                                                goto switchD_0001d064_caseD_1;
                                                              case 2:
                                                                goto switchD_0001d064_caseD_2;
                                                              case 3:
                                                                goto switchD_0001d064_caseD_3;
                                                              case 4:
                                                                goto switchD_0001d064_caseD_4;
                                                              case 5:
                                                                goto switchD_0001d064_caseD_5;
                                                              case 6:
                                                                goto switchD_0001d064_caseD_6;
                                                              case 7:
                                                                goto switchD_0001d064_caseD_7;
                                                              case 8:
                                                                goto switchD_0001d064_caseD_8;
                                                              case 9:
                                                                goto switchD_0001d064_caseD_9;
                                                              case 10:
                                                                goto switchD_0001d064_caseD_a;
                                                              case 0xb:
                                                                goto switchD_0001d064_caseD_b;
                                                              case 0xc:
                                                                goto switchD_0001d064_caseD_c;
                                                              case 0xd:
                                                                goto switchD_0001d064_caseD_d;
                                                              case 0xe:
                                                                goto switchD_0001d064_caseD_e;
                                                              case 0xf:
                                                                goto switchD_0001d064_caseD_f;
                                                              case 0x10:
                                                                goto switchD_0001d064_caseD_10;
                                                              case 0x11:
                                                                goto switchD_0001d064_caseD_11;
                                                              case 0x12:
                                                                goto switchD_0001d064_caseD_12;
                                                              case 0x13:
                                                                goto switchD_0001d064_caseD_13;
                                                              case 0x14:
                                                                goto switchD_0001d064_caseD_14;
                                                              case 0x15:
                                                                goto switchD_0001d064_caseD_15;
                                                              case 0x16:
                                                                goto switchD_0001d064_caseD_16;
                                                              case 0x17:
                                                                goto switchD_0001d064_caseD_17;
                                                              case 0x19:
switchD_0001d064_caseD_19:
                                                                do {
                                                                  uVar5 = CONCAT31((int3)((uint)*(
                                                  undefined4 *)((int)param_3 + param_4) >> 8),
                                                  (char)(uVar4 >> 8));
                                                  param_6 = (uint *)((int)param_6 + 2);
                                                  *param_3 = uVar5;
                                                  param_8 = param_8 + -1;
                                                  if (param_8 == 0) goto LAB_0001d06b;
                                                  param_3 = param_3 + 0x51;
                                                  param_4 = param_4 + _DAT_000a8df0;
                                                  uVar4 = *param_6;
                                                  puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                  switch(uVar4 & 0xff) {
                                                  case 0:
                                                    goto switchD_0001d064_caseD_0;
                                                  case 1:
                                                    goto switchD_0001d064_caseD_1;
                                                  case 2:
                                                    goto switchD_0001d064_caseD_2;
                                                  case 3:
                                                    goto switchD_0001d064_caseD_3;
                                                  case 4:
                                                    goto switchD_0001d064_caseD_4;
                                                  case 5:
                                                    goto switchD_0001d064_caseD_5;
                                                  case 6:
                                                    goto switchD_0001d064_caseD_6;
                                                  case 7:
                                                    goto switchD_0001d064_caseD_7;
                                                  case 8:
                                                    goto switchD_0001d064_caseD_8;
                                                  case 9:
                                                    goto switchD_0001d064_caseD_9;
                                                  case 10:
                                                    goto switchD_0001d064_caseD_a;
                                                  case 0xb:
                                                    goto switchD_0001d064_caseD_b;
                                                  case 0xc:
                                                    goto switchD_0001d064_caseD_c;
                                                  case 0xd:
                                                    goto switchD_0001d064_caseD_d;
                                                  case 0xe:
                                                    goto switchD_0001d064_caseD_e;
                                                  case 0xf:
                                                    goto switchD_0001d064_caseD_f;
                                                  case 0x10:
                                                    goto switchD_0001d064_caseD_10;
                                                  case 0x11:
                                                    goto switchD_0001d064_caseD_11;
                                                  case 0x12:
                                                    goto switchD_0001d064_caseD_12;
                                                  case 0x13:
                                                    goto switchD_0001d064_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d064_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d064_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d064_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d064_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d064_caseD_18;
                                                  case 0x1a:
switchD_0001d064_caseD_1a:
                                                    do {
                                                      uVar1 = (uint3)(uVar5 >> 8);
                                                      uVar5 = (uint)uVar1 << 8;
                                                      param_6 = param_6 + 1;
                                                      *param_3 = CONCAT22((short)(uVar4 >> 0x10),
                                                                          *(undefined2 *)
                                                                           ((int)param_3 +
                                                                           param_4 + 1)) << 8 |
                                                                 (uint)(uVar1 >> 0x10);
                                                      param_8 = param_8 + -1;
                                                      if (param_8 == 0) goto LAB_0001d06b;
                                                      param_3 = param_3 + 0x51;
                                                      param_4 = param_4 + _DAT_000a8df0;
                                                      uVar4 = *param_6;
                                                      puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                      switch(uVar4 & 0xff) {
                                                      case 0:
                                                        goto switchD_0001d064_caseD_0;
                                                      case 1:
                                                        goto switchD_0001d064_caseD_1;
                                                      case 2:
                                                        goto switchD_0001d064_caseD_2;
                                                      case 3:
                                                        goto switchD_0001d064_caseD_3;
                                                      case 4:
                                                        goto switchD_0001d064_caseD_4;
                                                      case 5:
                                                        goto switchD_0001d064_caseD_5;
                                                      case 6:
                                                        goto switchD_0001d064_caseD_6;
                                                      case 7:
                                                        goto switchD_0001d064_caseD_7;
                                                      case 8:
                                                        goto switchD_0001d064_caseD_8;
                                                      case 9:
                                                        goto switchD_0001d064_caseD_9;
                                                      case 10:
                                                        goto switchD_0001d064_caseD_a;
                                                      case 0xb:
                                                        goto switchD_0001d064_caseD_b;
                                                      case 0xc:
                                                        goto switchD_0001d064_caseD_c;
                                                      case 0xd:
                                                        goto switchD_0001d064_caseD_d;
                                                      case 0xe:
                                                        goto switchD_0001d064_caseD_e;
                                                      case 0xf:
                                                        goto switchD_0001d064_caseD_f;
                                                      case 0x10:
                                                        goto switchD_0001d064_caseD_10;
                                                      case 0x11:
                                                        goto switchD_0001d064_caseD_11;
                                                      case 0x12:
                                                        goto switchD_0001d064_caseD_12;
                                                      case 0x13:
                                                        goto switchD_0001d064_caseD_13;
                                                      case 0x14:
                                                        goto switchD_0001d064_caseD_14;
                                                      case 0x15:
                                                        goto switchD_0001d064_caseD_15;
                                                      case 0x16:
                                                        goto switchD_0001d064_caseD_16;
                                                      case 0x17:
                                                        goto switchD_0001d064_caseD_17;
                                                      case 0x18:
                                                        goto switchD_0001d064_caseD_18;
                                                      case 0x19:
                                                        goto switchD_0001d064_caseD_19;
                                                      case 0x1b:
switchD_0001d064_caseD_1b:
                                                        do {
                                                          param_6 = param_6 + 1;
                                                          *param_3 = CONCAT22((ushort)(byte)(uVar4 
                                                  >> 0x18) |
                                                  (ushort)(((uint)*(byte *)((int)param_3 +
                                                                           param_4 + 3) << 0x18) >>
                                                          0x10),
                                                  CONCAT11(*(undefined1 *)
                                                            ((int)param_3 + param_4 + 1),
                                                           (char)(uVar4 >> 8)));
                                                  param_8 = param_8 + -1;
                                                  if (param_8 == 0) goto LAB_0001d06b;
                                                  param_3 = param_3 + 0x51;
                                                  param_4 = param_4 + _DAT_000a8df0;
                                                  uVar4 = *param_6;
                                                  puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                  switch(uVar4 & 0xff) {
                                                  case 0:
                                                    goto switchD_0001d064_caseD_0;
                                                  case 1:
                                                    goto switchD_0001d064_caseD_1;
                                                  case 2:
                                                    goto switchD_0001d064_caseD_2;
                                                  case 3:
                                                    goto switchD_0001d064_caseD_3;
                                                  case 4:
                                                    goto switchD_0001d064_caseD_4;
                                                  case 5:
                                                    goto switchD_0001d064_caseD_5;
                                                  case 6:
                                                    goto switchD_0001d064_caseD_6;
                                                  case 7:
                                                    goto switchD_0001d064_caseD_7;
                                                  case 8:
                                                    goto switchD_0001d064_caseD_8;
                                                  case 9:
                                                    goto switchD_0001d064_caseD_9;
                                                  case 10:
                                                    goto switchD_0001d064_caseD_a;
                                                  case 0xb:
                                                    goto switchD_0001d064_caseD_b;
                                                  case 0xc:
                                                    goto switchD_0001d064_caseD_c;
                                                  case 0xd:
                                                    goto switchD_0001d064_caseD_d;
                                                  case 0xe:
                                                    goto switchD_0001d064_caseD_e;
                                                  case 0xf:
                                                    goto switchD_0001d064_caseD_f;
                                                  case 0x10:
                                                    goto switchD_0001d064_caseD_10;
                                                  case 0x11:
                                                    goto switchD_0001d064_caseD_11;
                                                  case 0x12:
                                                    goto switchD_0001d064_caseD_12;
                                                  case 0x13:
                                                    goto switchD_0001d064_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d064_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d064_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d064_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d064_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d064_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d064_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d064_caseD_1a;
                                                  case 0x1c:
switchD_0001d064_caseD_1c:
                                                    do {
                                                      param_6 = param_6 + 1;
                                                      *param_3 = CONCAT22((short)(uVar4 >> 0x10),
                                                                          CONCAT11(*(undefined1 *)
                                                                                    ((int)param_3 +
                                                                                    param_4 + 1),
                                                                                   (char)(uVar4 >> 8
                                                                                         )));
                                                      param_8 = param_8 + -1;
                                                      if (param_8 == 0) goto LAB_0001d06b;
                                                      param_3 = param_3 + 0x51;
                                                      param_4 = param_4 + _DAT_000a8df0;
                                                      uVar4 = *param_6;
                                                      puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                      switch(uVar4 & 0xff) {
                                                      case 0:
                                                        goto switchD_0001d064_caseD_0;
                                                      case 1:
                                                        goto switchD_0001d064_caseD_1;
                                                      case 2:
                                                        goto switchD_0001d064_caseD_2;
                                                      case 3:
                                                        goto switchD_0001d064_caseD_3;
                                                      case 4:
                                                        goto switchD_0001d064_caseD_4;
                                                      case 5:
                                                        goto switchD_0001d064_caseD_5;
                                                      case 6:
                                                        goto switchD_0001d064_caseD_6;
                                                      case 7:
                                                        goto switchD_0001d064_caseD_7;
                                                      case 8:
                                                        goto switchD_0001d064_caseD_8;
                                                      case 9:
                                                        goto switchD_0001d064_caseD_9;
                                                      case 10:
                                                        goto switchD_0001d064_caseD_a;
                                                      case 0xb:
                                                        goto switchD_0001d064_caseD_b;
                                                      case 0xc:
                                                        goto switchD_0001d064_caseD_c;
                                                      case 0xd:
                                                        goto switchD_0001d064_caseD_d;
                                                      case 0xe:
                                                        goto switchD_0001d064_caseD_e;
                                                      case 0xf:
                                                        goto switchD_0001d064_caseD_f;
                                                      case 0x10:
                                                        goto switchD_0001d064_caseD_10;
                                                      case 0x11:
                                                        goto switchD_0001d064_caseD_11;
                                                      case 0x12:
                                                        goto switchD_0001d064_caseD_12;
                                                      case 0x13:
                                                        goto switchD_0001d064_caseD_13;
                                                      case 0x14:
                                                        goto switchD_0001d064_caseD_14;
                                                      case 0x15:
                                                        goto switchD_0001d064_caseD_15;
                                                      case 0x16:
                                                        goto switchD_0001d064_caseD_16;
                                                      case 0x17:
                                                        goto switchD_0001d064_caseD_17;
                                                      case 0x18:
                                                        goto switchD_0001d064_caseD_18;
                                                      case 0x19:
                                                        goto switchD_0001d064_caseD_19;
                                                      case 0x1a:
                                                        goto switchD_0001d064_caseD_1a;
                                                      case 0x1b:
                                                        goto switchD_0001d064_caseD_1b;
                                                      case 0x1d:
switchD_0001d064_caseD_1d:
                                                        do {
                                                          uVar2 = (ushort)(uVar4 >> 0x10);
                                                          param_6 = param_6 + 1;
                                                          *param_3 = (uint)uVar2 |
                                                                     CONCAT22(uVar2,*(undefined2 *)
                                                                                     ((int)param_3 +
                                                                                     param_4 + 2))
                                                                     << 0x10;
                                                          param_8 = param_8 + -1;
                                                          if (param_8 == 0) goto LAB_0001d06b;
                                                          param_3 = param_3 + 0x51;
                                                          param_4 = param_4 + _DAT_000a8df0;
                                                          uVar4 = *param_6;
                                                          puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                          switch(uVar4 & 0xff) {
                                                          case 0:
                                                            goto switchD_0001d064_caseD_0;
                                                          case 1:
                                                            goto switchD_0001d064_caseD_1;
                                                          case 2:
                                                            goto switchD_0001d064_caseD_2;
                                                          case 3:
                                                            goto switchD_0001d064_caseD_3;
                                                          case 4:
                                                            goto switchD_0001d064_caseD_4;
                                                          case 5:
                                                            goto switchD_0001d064_caseD_5;
                                                          case 6:
                                                            goto switchD_0001d064_caseD_6;
                                                          case 7:
                                                            goto switchD_0001d064_caseD_7;
                                                          case 8:
                                                            goto switchD_0001d064_caseD_8;
                                                          case 9:
                                                            goto switchD_0001d064_caseD_9;
                                                          case 10:
                                                            goto switchD_0001d064_caseD_a;
                                                          case 0xb:
                                                            goto switchD_0001d064_caseD_b;
                                                          case 0xc:
                                                            goto switchD_0001d064_caseD_c;
                                                          case 0xd:
                                                            goto switchD_0001d064_caseD_d;
                                                          case 0xe:
                                                            goto switchD_0001d064_caseD_e;
                                                          case 0xf:
                                                            goto switchD_0001d064_caseD_f;
                                                          case 0x10:
                                                            goto switchD_0001d064_caseD_10;
                                                          case 0x11:
                                                            goto switchD_0001d064_caseD_11;
                                                          case 0x12:
                                                            goto switchD_0001d064_caseD_12;
                                                          case 0x13:
                                                            goto switchD_0001d064_caseD_13;
                                                          case 0x14:
                                                            goto switchD_0001d064_caseD_14;
                                                          case 0x15:
                                                            goto switchD_0001d064_caseD_15;
                                                          case 0x16:
                                                            goto switchD_0001d064_caseD_16;
                                                          case 0x17:
                                                            goto switchD_0001d064_caseD_17;
                                                          case 0x18:
                                                            goto switchD_0001d064_caseD_18;
                                                          case 0x19:
                                                            goto switchD_0001d064_caseD_19;
                                                          case 0x1a:
                                                            goto switchD_0001d064_caseD_1a;
                                                          case 0x1b:
                                                            goto switchD_0001d064_caseD_1b;
                                                          case 0x1c:
                                                            goto switchD_0001d064_caseD_1c;
                                                          case 0x1e:
switchD_0001d064_caseD_1e:
                                                            do {
                                                              uVar1 = (uint3)(uVar4 >> 8);
                                                              param_6 = param_6 + 1;
                                                              *param_3 = CONCAT31(uVar1,*(undefined1
                                                                                          *)((int)
                                                  param_3 + param_4 + 2)) << 0x10 |
                                                  (uint)(uVar1 >> 8);
                                                  param_8 = param_8 + -1;
                                                  if (param_8 == 0) goto LAB_0001d06b;
                                                  param_3 = param_3 + 0x51;
                                                  param_4 = param_4 + _DAT_000a8df0;
                                                  uVar4 = *param_6;
                                                  puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                  switch(uVar4 & 0xff) {
                                                  case 0:
                                                    goto switchD_0001d064_caseD_0;
                                                  case 1:
                                                    goto switchD_0001d064_caseD_1;
                                                  case 2:
                                                    goto switchD_0001d064_caseD_2;
                                                  case 3:
                                                    goto switchD_0001d064_caseD_3;
                                                  case 4:
                                                    goto switchD_0001d064_caseD_4;
                                                  case 5:
                                                    goto switchD_0001d064_caseD_5;
                                                  case 6:
                                                    goto switchD_0001d064_caseD_6;
                                                  case 7:
                                                    goto switchD_0001d064_caseD_7;
                                                  case 8:
                                                    goto switchD_0001d064_caseD_8;
                                                  case 9:
                                                    goto switchD_0001d064_caseD_9;
                                                  case 10:
                                                    goto switchD_0001d064_caseD_a;
                                                  case 0xb:
                                                    goto switchD_0001d064_caseD_b;
                                                  case 0xc:
                                                    goto switchD_0001d064_caseD_c;
                                                  case 0xd:
                                                    goto switchD_0001d064_caseD_d;
                                                  case 0xe:
                                                    goto switchD_0001d064_caseD_e;
                                                  case 0xf:
                                                    goto switchD_0001d064_caseD_f;
                                                  case 0x10:
                                                    goto switchD_0001d064_caseD_10;
                                                  case 0x11:
                                                    goto switchD_0001d064_caseD_11;
                                                  case 0x12:
                                                    goto switchD_0001d064_caseD_12;
                                                  case 0x13:
                                                    goto switchD_0001d064_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d064_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d064_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d064_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d064_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d064_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d064_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d064_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d064_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d064_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d064_caseD_1d;
                                                  case 0x1f:
switchD_0001d064_caseD_1f:
                                                    do {
                                                      uVar1 = (uint3)(uVar4 >> 8);
                                                      param_6 = param_6 + 1;
                                                      *param_3 = (uint)uVar1 |
                                                                 CONCAT31(uVar1,*(undefined1 *)
                                                                                 ((int)param_3 +
                                                                                 param_4 + 3)) <<
                                                                 0x18;
                                                      param_8 = param_8 + -1;
                                                      if (param_8 == 0) goto LAB_0001d06b;
                                                      param_3 = param_3 + 0x51;
                                                      param_4 = param_4 + _DAT_000a8df0;
                                                      uVar4 = *param_6;
                                                      puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                      switch(uVar4 & 0xff) {
                                                      case 0:
                                                        goto switchD_0001d064_caseD_0;
                                                      case 1:
                                                        goto switchD_0001d064_caseD_1;
                                                      case 2:
                                                        goto switchD_0001d064_caseD_2;
                                                      case 3:
                                                        goto switchD_0001d064_caseD_3;
                                                      case 4:
                                                        goto switchD_0001d064_caseD_4;
                                                      case 5:
                                                        goto switchD_0001d064_caseD_5;
                                                      case 6:
                                                        goto switchD_0001d064_caseD_6;
                                                      case 7:
                                                        goto switchD_0001d064_caseD_7;
                                                      case 8:
                                                        goto switchD_0001d064_caseD_8;
                                                      case 9:
                                                        goto switchD_0001d064_caseD_9;
                                                      case 10:
                                                        goto switchD_0001d064_caseD_a;
                                                      case 0xb:
                                                        goto switchD_0001d064_caseD_b;
                                                      case 0xc:
                                                        goto switchD_0001d064_caseD_c;
                                                      case 0xd:
                                                        goto switchD_0001d064_caseD_d;
                                                      case 0xe:
                                                        goto switchD_0001d064_caseD_e;
                                                      case 0xf:
                                                        goto switchD_0001d064_caseD_f;
                                                      case 0x10:
                                                        goto switchD_0001d064_caseD_10;
                                                      case 0x11:
                                                        goto switchD_0001d064_caseD_11;
                                                      case 0x12:
                                                        goto switchD_0001d064_caseD_12;
                                                      case 0x13:
                                                        goto switchD_0001d064_caseD_13;
                                                      case 0x14:
                                                        goto switchD_0001d064_caseD_14;
                                                      case 0x15:
                                                        goto switchD_0001d064_caseD_15;
                                                      case 0x16:
                                                        goto switchD_0001d064_caseD_16;
                                                      case 0x17:
                                                        goto switchD_0001d064_caseD_17;
                                                      case 0x18:
                                                        goto switchD_0001d064_caseD_18;
                                                      case 0x19:
                                                        goto switchD_0001d064_caseD_19;
                                                      case 0x1a:
                                                        goto switchD_0001d064_caseD_1a;
                                                      case 0x1b:
                                                        goto switchD_0001d064_caseD_1b;
                                                      case 0x1c:
                                                        goto switchD_0001d064_caseD_1c;
                                                      case 0x1d:
                                                        goto switchD_0001d064_caseD_1d;
                                                      case 0x1e:
                                                        goto switchD_0001d064_caseD_1e;
                                                      case 0x20:
switchD_0001d064_caseD_20:
                                                        do {
                                                          param_6 = (uint *)((int)param_6 + 2);
                                                          for (uVar4 = uVar4 >> 8 & 0xff; uVar4 != 0
                                                              ; uVar4 = uVar4 - 1) {
                                                            *param_3 = *param_6;
                                                            param_6 = param_6 + 1;
                                                            param_3 = param_3 + 1;
                                                          }
                                                          param_8 = param_8 + -1;
                                                          if (param_8 == 0) goto LAB_0001d06b;
                                                          param_3 = param_3 + 0x50;
                                                          param_4 = param_4 + _DAT_000a8df0;
                                                          uVar4 = *param_6;
                                                          puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                          switch(uVar4 & 0xff) {
                                                          case 0:
                                                            goto switchD_0001d064_caseD_0;
                                                          case 1:
                                                            goto switchD_0001d064_caseD_1;
                                                          case 2:
                                                            goto switchD_0001d064_caseD_2;
                                                          case 3:
                                                            goto switchD_0001d064_caseD_3;
                                                          case 4:
                                                            goto switchD_0001d064_caseD_4;
                                                          case 5:
                                                            goto switchD_0001d064_caseD_5;
                                                          case 6:
                                                            goto switchD_0001d064_caseD_6;
                                                          case 7:
                                                            goto switchD_0001d064_caseD_7;
                                                          case 8:
                                                            goto switchD_0001d064_caseD_8;
                                                          case 9:
                                                            goto switchD_0001d064_caseD_9;
                                                          case 10:
                                                            goto switchD_0001d064_caseD_a;
                                                          case 0xb:
                                                            goto switchD_0001d064_caseD_b;
                                                          case 0xc:
                                                            goto switchD_0001d064_caseD_c;
                                                          case 0xd:
                                                            goto switchD_0001d064_caseD_d;
                                                          case 0xe:
                                                            goto switchD_0001d064_caseD_e;
                                                          case 0xf:
                                                            goto switchD_0001d064_caseD_f;
                                                          case 0x10:
                                                            goto switchD_0001d064_caseD_10;
                                                          case 0x11:
                                                            goto switchD_0001d064_caseD_11;
                                                          case 0x12:
                                                            goto switchD_0001d064_caseD_12;
                                                          case 0x13:
                                                            goto switchD_0001d064_caseD_13;
                                                          case 0x14:
                                                            goto switchD_0001d064_caseD_14;
                                                          case 0x15:
                                                            goto switchD_0001d064_caseD_15;
                                                          case 0x16:
                                                            goto switchD_0001d064_caseD_16;
                                                          case 0x17:
                                                            goto switchD_0001d064_caseD_17;
                                                          case 0x18:
                                                            goto switchD_0001d064_caseD_18;
                                                          case 0x19:
                                                            goto switchD_0001d064_caseD_19;
                                                          case 0x1a:
                                                            goto switchD_0001d064_caseD_1a;
                                                          case 0x1b:
                                                            goto switchD_0001d064_caseD_1b;
                                                          case 0x1c:
                                                            goto switchD_0001d064_caseD_1c;
                                                          case 0x1d:
                                                            goto switchD_0001d064_caseD_1d;
                                                          case 0x1e:
                                                            goto switchD_0001d064_caseD_1e;
                                                          case 0x1f:
                                                            goto switchD_0001d064_caseD_1f;
                                                          case 0x21:
switchD_0001d064_caseD_21:
                                                            do {
                                                              uVar5 = uVar5 << 0x10 | uVar4 >> 0x10;
                                                              uVar3 = (undefined2)(uVar4 >> 0x10);
                                                              for (uVar4 = uVar4 >> 8 & 0xff;
                                                                  uVar4 != 0; uVar4 = uVar4 - 1) {
                                                                *param_3 = CONCAT22(uVar3,uVar3);
                                                                param_3 = param_3 + 1;
                                                              }
                                                              param_6 = param_6 + 1;
                                                              param_8 = param_8 + -1;
                                                              if (param_8 == 0) goto LAB_0001d06b;
                                                              param_3 = param_3 + 0x50;
                                                              param_4 = param_4 + _DAT_000a8df0;
                                                              uVar4 = *param_6;
                                                              puVar6 = param_3;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                              switch(uVar4 & 0xff) {
                                                              case 0:
                                                                goto switchD_0001d064_caseD_0;
                                                              case 1:
                                                                goto switchD_0001d064_caseD_1;
                                                              case 2:
                                                                goto switchD_0001d064_caseD_2;
                                                              case 3:
                                                                goto switchD_0001d064_caseD_3;
                                                              case 4:
                                                                goto switchD_0001d064_caseD_4;
                                                              case 5:
                                                                goto switchD_0001d064_caseD_5;
                                                              case 6:
                                                                goto switchD_0001d064_caseD_6;
                                                              case 7:
                                                                goto switchD_0001d064_caseD_7;
                                                              case 8:
                                                                goto switchD_0001d064_caseD_8;
                                                              case 9:
                                                                goto switchD_0001d064_caseD_9;
                                                              case 10:
                                                                goto switchD_0001d064_caseD_a;
                                                              case 0xb:
                                                                goto switchD_0001d064_caseD_b;
                                                              case 0xc:
                                                                goto switchD_0001d064_caseD_c;
                                                              case 0xd:
                                                                goto switchD_0001d064_caseD_d;
                                                              case 0xe:
                                                                goto switchD_0001d064_caseD_e;
                                                              case 0xf:
                                                                goto switchD_0001d064_caseD_f;
                                                              case 0x10:
                                                                goto switchD_0001d064_caseD_10;
                                                              case 0x11:
                                                                goto switchD_0001d064_caseD_11;
                                                              case 0x12:
                                                                goto switchD_0001d064_caseD_12;
                                                              case 0x13:
                                                                goto switchD_0001d064_caseD_13;
                                                              case 0x14:
                                                                goto switchD_0001d064_caseD_14;
                                                              case 0x15:
                                                                goto switchD_0001d064_caseD_15;
                                                              case 0x16:
                                                                goto switchD_0001d064_caseD_16;
                                                              case 0x17:
                                                                goto switchD_0001d064_caseD_17;
                                                              case 0x18:
                                                                goto switchD_0001d064_caseD_18;
                                                              case 0x19:
                                                                goto switchD_0001d064_caseD_19;
                                                              case 0x1a:
                                                                goto switchD_0001d064_caseD_1a;
                                                              case 0x1b:
                                                                goto switchD_0001d064_caseD_1b;
                                                              case 0x1c:
                                                                goto switchD_0001d064_caseD_1c;
                                                              case 0x1d:
                                                                goto switchD_0001d064_caseD_1d;
                                                              case 0x1e:
                                                                goto switchD_0001d064_caseD_1e;
                                                              case 0x1f:
                                                                goto switchD_0001d064_caseD_1f;
                                                              case 0x20:
                                                                goto switchD_0001d064_caseD_20;
                                                              }
                                                            } while( true );
                                                          }
                                                        } while( true );
                                                      case 0x21:
                                                        goto switchD_0001d064_caseD_21;
                                                      }
                                                    } while( true );
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x1e:
                                                    goto switchD_0001d064_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x1d:
                                                    goto switchD_0001d064_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d064_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x1c:
                                                    goto switchD_0001d064_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d064_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d064_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x1b:
                                                    goto switchD_0001d064_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d064_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d064_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d064_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x1a:
                                                    goto switchD_0001d064_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d064_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d064_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d064_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d064_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x19:
                                                    goto switchD_0001d064_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d064_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d064_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d064_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d064_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d064_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x18:
                                                    goto switchD_0001d064_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d064_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d064_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d064_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d064_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d064_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d064_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x17:
                                                    goto switchD_0001d064_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d064_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d064_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d064_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d064_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d064_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d064_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d064_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x16:
                                                    goto switchD_0001d064_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d064_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d064_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d064_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d064_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d064_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d064_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d064_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d064_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x15:
                                                    goto switchD_0001d064_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d064_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d064_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d064_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d064_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d064_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d064_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d064_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d064_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d064_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x14:
                                                    goto switchD_0001d064_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d064_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d064_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d064_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d064_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d064_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d064_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d064_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d064_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d064_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d064_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x13:
                                                    goto switchD_0001d064_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d064_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d064_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d064_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d064_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d064_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d064_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d064_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d064_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d064_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d064_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d064_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x12:
                                                    goto switchD_0001d064_caseD_12;
                                                  case 0x13:
                                                    goto switchD_0001d064_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d064_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d064_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d064_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d064_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d064_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d064_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d064_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d064_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d064_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d064_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d064_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x11:
                                                    goto switchD_0001d064_caseD_11;
                                                  case 0x12:
                                                    goto switchD_0001d064_caseD_12;
                                                  case 0x13:
                                                    goto switchD_0001d064_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d064_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d064_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d064_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d064_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d064_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d064_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d064_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d064_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d064_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d064_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d064_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x10:
                                                    goto switchD_0001d064_caseD_10;
                                                  case 0x11:
                                                    goto switchD_0001d064_caseD_11;
                                                  case 0x12:
                                                    goto switchD_0001d064_caseD_12;
                                                  case 0x13:
                                                    goto switchD_0001d064_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d064_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d064_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d064_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d064_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d064_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d064_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d064_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d064_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d064_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d064_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d064_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0xf:
                                                    goto switchD_0001d064_caseD_f;
                                                  case 0x10:
                                                    goto switchD_0001d064_caseD_10;
                                                  case 0x11:
                                                    goto switchD_0001d064_caseD_11;
                                                  case 0x12:
                                                    goto switchD_0001d064_caseD_12;
                                                  case 0x13:
                                                    goto switchD_0001d064_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d064_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d064_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d064_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d064_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d064_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d064_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d064_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d064_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d064_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d064_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d064_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0xe:
                                                    goto switchD_0001d064_caseD_e;
                                                  case 0xf:
                                                    goto switchD_0001d064_caseD_f;
                                                  case 0x10:
                                                    goto switchD_0001d064_caseD_10;
                                                  case 0x11:
                                                    goto switchD_0001d064_caseD_11;
                                                  case 0x12:
                                                    goto switchD_0001d064_caseD_12;
                                                  case 0x13:
                                                    goto switchD_0001d064_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d064_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d064_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d064_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d064_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d064_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d064_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d064_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d064_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d064_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d064_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d064_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d064_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d064_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d064_caseD_21;
                                                  }
                                                  } while( true );
                                                case 0xd:
                                                  goto switchD_0001d064_caseD_d;
                                                case 0xe:
                                                  goto switchD_0001d064_caseD_e;
                                                case 0xf:
                                                  goto switchD_0001d064_caseD_f;
                                                case 0x10:
                                                  goto switchD_0001d064_caseD_10;
                                                case 0x11:
                                                  goto switchD_0001d064_caseD_11;
                                                case 0x12:
                                                  goto switchD_0001d064_caseD_12;
                                                case 0x13:
                                                  goto switchD_0001d064_caseD_13;
                                                case 0x14:
                                                  goto switchD_0001d064_caseD_14;
                                                case 0x15:
                                                  goto switchD_0001d064_caseD_15;
                                                case 0x16:
                                                  goto switchD_0001d064_caseD_16;
                                                case 0x17:
                                                  goto switchD_0001d064_caseD_17;
                                                case 0x18:
                                                  goto switchD_0001d064_caseD_18;
                                                case 0x19:
                                                  goto switchD_0001d064_caseD_19;
                                                case 0x1a:
                                                  goto switchD_0001d064_caseD_1a;
                                                case 0x1b:
                                                  goto switchD_0001d064_caseD_1b;
                                                case 0x1c:
                                                  goto switchD_0001d064_caseD_1c;
                                                case 0x1d:
                                                  goto switchD_0001d064_caseD_1d;
                                                case 0x1e:
                                                  goto switchD_0001d064_caseD_1e;
                                                case 0x1f:
                                                  goto switchD_0001d064_caseD_1f;
                                                case 0x20:
                                                  goto switchD_0001d064_caseD_20;
                                                case 0x21:
                                                  goto switchD_0001d064_caseD_21;
                                                }
                                              } while( true );
                                            case 0xc:
                                              goto switchD_0001d064_caseD_c;
                                            case 0xd:
                                              goto switchD_0001d064_caseD_d;
                                            case 0xe:
                                              goto switchD_0001d064_caseD_e;
                                            case 0xf:
                                              goto switchD_0001d064_caseD_f;
                                            case 0x10:
                                              goto switchD_0001d064_caseD_10;
                                            case 0x11:
                                              goto switchD_0001d064_caseD_11;
                                            case 0x12:
                                              goto switchD_0001d064_caseD_12;
                                            case 0x13:
                                              goto switchD_0001d064_caseD_13;
                                            case 0x14:
                                              goto switchD_0001d064_caseD_14;
                                            case 0x15:
                                              goto switchD_0001d064_caseD_15;
                                            case 0x16:
                                              goto switchD_0001d064_caseD_16;
                                            case 0x17:
                                              goto switchD_0001d064_caseD_17;
                                            case 0x18:
                                              goto switchD_0001d064_caseD_18;
                                            case 0x19:
                                              goto switchD_0001d064_caseD_19;
                                            case 0x1a:
                                              goto switchD_0001d064_caseD_1a;
                                            case 0x1b:
                                              goto switchD_0001d064_caseD_1b;
                                            case 0x1c:
                                              goto switchD_0001d064_caseD_1c;
                                            case 0x1d:
                                              goto switchD_0001d064_caseD_1d;
                                            case 0x1e:
                                              goto switchD_0001d064_caseD_1e;
                                            case 0x1f:
                                              goto switchD_0001d064_caseD_1f;
                                            case 0x20:
                                              goto switchD_0001d064_caseD_20;
                                            case 0x21:
                                              goto switchD_0001d064_caseD_21;
                                            }
                                          } while( true );
                                        case 0xb:
                                          goto switchD_0001d064_caseD_b;
                                        case 0xc:
                                          goto switchD_0001d064_caseD_c;
                                        case 0xd:
                                          goto switchD_0001d064_caseD_d;
                                        case 0xe:
                                          goto switchD_0001d064_caseD_e;
                                        case 0xf:
                                          goto switchD_0001d064_caseD_f;
                                        case 0x10:
                                          goto switchD_0001d064_caseD_10;
                                        case 0x11:
                                          goto switchD_0001d064_caseD_11;
                                        case 0x12:
                                          goto switchD_0001d064_caseD_12;
                                        case 0x13:
                                          goto switchD_0001d064_caseD_13;
                                        case 0x14:
                                          goto switchD_0001d064_caseD_14;
                                        case 0x15:
                                          goto switchD_0001d064_caseD_15;
                                        case 0x16:
                                          goto switchD_0001d064_caseD_16;
                                        case 0x17:
                                          goto switchD_0001d064_caseD_17;
                                        case 0x18:
                                          goto switchD_0001d064_caseD_18;
                                        case 0x19:
                                          goto switchD_0001d064_caseD_19;
                                        case 0x1a:
                                          goto switchD_0001d064_caseD_1a;
                                        case 0x1b:
                                          goto switchD_0001d064_caseD_1b;
                                        case 0x1c:
                                          goto switchD_0001d064_caseD_1c;
                                        case 0x1d:
                                          goto switchD_0001d064_caseD_1d;
                                        case 0x1e:
                                          goto switchD_0001d064_caseD_1e;
                                        case 0x1f:
                                          goto switchD_0001d064_caseD_1f;
                                        case 0x20:
                                          goto switchD_0001d064_caseD_20;
                                        case 0x21:
                                          goto switchD_0001d064_caseD_21;
                                        }
                                      } while( true );
                                    case 10:
                                      goto switchD_0001d064_caseD_a;
                                    case 0xb:
                                      goto switchD_0001d064_caseD_b;
                                    case 0xc:
                                      goto switchD_0001d064_caseD_c;
                                    case 0xd:
                                      goto switchD_0001d064_caseD_d;
                                    case 0xe:
                                      goto switchD_0001d064_caseD_e;
                                    case 0xf:
                                      goto switchD_0001d064_caseD_f;
                                    case 0x10:
                                      goto switchD_0001d064_caseD_10;
                                    case 0x11:
                                      goto switchD_0001d064_caseD_11;
                                    case 0x12:
                                      goto switchD_0001d064_caseD_12;
                                    case 0x13:
                                      goto switchD_0001d064_caseD_13;
                                    case 0x14:
                                      goto switchD_0001d064_caseD_14;
                                    case 0x15:
                                      goto switchD_0001d064_caseD_15;
                                    case 0x16:
                                      goto switchD_0001d064_caseD_16;
                                    case 0x17:
                                      goto switchD_0001d064_caseD_17;
                                    case 0x18:
                                      goto switchD_0001d064_caseD_18;
                                    case 0x19:
                                      goto switchD_0001d064_caseD_19;
                                    case 0x1a:
                                      goto switchD_0001d064_caseD_1a;
                                    case 0x1b:
                                      goto switchD_0001d064_caseD_1b;
                                    case 0x1c:
                                      goto switchD_0001d064_caseD_1c;
                                    case 0x1d:
                                      goto switchD_0001d064_caseD_1d;
                                    case 0x1e:
                                      goto switchD_0001d064_caseD_1e;
                                    case 0x1f:
                                      goto switchD_0001d064_caseD_1f;
                                    case 0x20:
                                      goto switchD_0001d064_caseD_20;
                                    case 0x21:
                                      goto switchD_0001d064_caseD_21;
                                    }
                                  } while( true );
                                case 9:
                                  goto switchD_0001d064_caseD_9;
                                case 10:
                                  goto switchD_0001d064_caseD_a;
                                case 0xb:
                                  goto switchD_0001d064_caseD_b;
                                case 0xc:
                                  goto switchD_0001d064_caseD_c;
                                case 0xd:
                                  goto switchD_0001d064_caseD_d;
                                case 0xe:
                                  goto switchD_0001d064_caseD_e;
                                case 0xf:
                                  goto switchD_0001d064_caseD_f;
                                case 0x10:
                                  goto switchD_0001d064_caseD_10;
                                case 0x11:
                                  goto switchD_0001d064_caseD_11;
                                case 0x12:
                                  goto switchD_0001d064_caseD_12;
                                case 0x13:
                                  goto switchD_0001d064_caseD_13;
                                case 0x14:
                                  goto switchD_0001d064_caseD_14;
                                case 0x15:
                                  goto switchD_0001d064_caseD_15;
                                case 0x16:
                                  goto switchD_0001d064_caseD_16;
                                case 0x17:
                                  goto switchD_0001d064_caseD_17;
                                case 0x18:
                                  goto switchD_0001d064_caseD_18;
                                case 0x19:
                                  goto switchD_0001d064_caseD_19;
                                case 0x1a:
                                  goto switchD_0001d064_caseD_1a;
                                case 0x1b:
                                  goto switchD_0001d064_caseD_1b;
                                case 0x1c:
                                  goto switchD_0001d064_caseD_1c;
                                case 0x1d:
                                  goto switchD_0001d064_caseD_1d;
                                case 0x1e:
                                  goto switchD_0001d064_caseD_1e;
                                case 0x1f:
                                  goto switchD_0001d064_caseD_1f;
                                case 0x20:
                                  goto switchD_0001d064_caseD_20;
                                case 0x21:
                                  goto switchD_0001d064_caseD_21;
                                }
                              } while( true );
                            case 8:
                              goto switchD_0001d064_caseD_8;
                            case 9:
                              goto switchD_0001d064_caseD_9;
                            case 10:
                              goto switchD_0001d064_caseD_a;
                            case 0xb:
                              goto switchD_0001d064_caseD_b;
                            case 0xc:
                              goto switchD_0001d064_caseD_c;
                            case 0xd:
                              goto switchD_0001d064_caseD_d;
                            case 0xe:
                              goto switchD_0001d064_caseD_e;
                            case 0xf:
                              goto switchD_0001d064_caseD_f;
                            case 0x10:
                              goto switchD_0001d064_caseD_10;
                            case 0x11:
                              goto switchD_0001d064_caseD_11;
                            case 0x12:
                              goto switchD_0001d064_caseD_12;
                            case 0x13:
                              goto switchD_0001d064_caseD_13;
                            case 0x14:
                              goto switchD_0001d064_caseD_14;
                            case 0x15:
                              goto switchD_0001d064_caseD_15;
                            case 0x16:
                              goto switchD_0001d064_caseD_16;
                            case 0x17:
                              goto switchD_0001d064_caseD_17;
                            case 0x18:
                              goto switchD_0001d064_caseD_18;
                            case 0x19:
                              goto switchD_0001d064_caseD_19;
                            case 0x1a:
                              goto switchD_0001d064_caseD_1a;
                            case 0x1b:
                              goto switchD_0001d064_caseD_1b;
                            case 0x1c:
                              goto switchD_0001d064_caseD_1c;
                            case 0x1d:
                              goto switchD_0001d064_caseD_1d;
                            case 0x1e:
                              goto switchD_0001d064_caseD_1e;
                            case 0x1f:
                              goto switchD_0001d064_caseD_1f;
                            case 0x20:
                              goto switchD_0001d064_caseD_20;
                            case 0x21:
                              goto switchD_0001d064_caseD_21;
                            }
                          } while( true );
                        case 7:
                          goto switchD_0001d064_caseD_7;
                        case 8:
                          goto switchD_0001d064_caseD_8;
                        case 9:
                          goto switchD_0001d064_caseD_9;
                        case 10:
                          goto switchD_0001d064_caseD_a;
                        case 0xb:
                          goto switchD_0001d064_caseD_b;
                        case 0xc:
                          goto switchD_0001d064_caseD_c;
                        case 0xd:
                          goto switchD_0001d064_caseD_d;
                        case 0xe:
                          goto switchD_0001d064_caseD_e;
                        case 0xf:
                          goto switchD_0001d064_caseD_f;
                        case 0x10:
                          goto switchD_0001d064_caseD_10;
                        case 0x11:
                          goto switchD_0001d064_caseD_11;
                        case 0x12:
                          goto switchD_0001d064_caseD_12;
                        case 0x13:
                          goto switchD_0001d064_caseD_13;
                        case 0x14:
                          goto switchD_0001d064_caseD_14;
                        case 0x15:
                          goto switchD_0001d064_caseD_15;
                        case 0x16:
                          goto switchD_0001d064_caseD_16;
                        case 0x17:
                          goto switchD_0001d064_caseD_17;
                        case 0x18:
                          goto switchD_0001d064_caseD_18;
                        case 0x19:
                          goto switchD_0001d064_caseD_19;
                        case 0x1a:
                          goto switchD_0001d064_caseD_1a;
                        case 0x1b:
                          goto switchD_0001d064_caseD_1b;
                        case 0x1c:
                          goto switchD_0001d064_caseD_1c;
                        case 0x1d:
                          goto switchD_0001d064_caseD_1d;
                        case 0x1e:
                          goto switchD_0001d064_caseD_1e;
                        case 0x1f:
                          goto switchD_0001d064_caseD_1f;
                        case 0x20:
                          goto switchD_0001d064_caseD_20;
                        case 0x21:
                          goto switchD_0001d064_caseD_21;
                        }
                      } while( true );
                    case 6:
                      goto switchD_0001d064_caseD_6;
                    case 7:
                      goto switchD_0001d064_caseD_7;
                    case 8:
                      goto switchD_0001d064_caseD_8;
                    case 9:
                      goto switchD_0001d064_caseD_9;
                    case 10:
                      goto switchD_0001d064_caseD_a;
                    case 0xb:
                      goto switchD_0001d064_caseD_b;
                    case 0xc:
                      goto switchD_0001d064_caseD_c;
                    case 0xd:
                      goto switchD_0001d064_caseD_d;
                    case 0xe:
                      goto switchD_0001d064_caseD_e;
                    case 0xf:
                      goto switchD_0001d064_caseD_f;
                    case 0x10:
                      goto switchD_0001d064_caseD_10;
                    case 0x11:
                      goto switchD_0001d064_caseD_11;
                    case 0x12:
                      goto switchD_0001d064_caseD_12;
                    case 0x13:
                      goto switchD_0001d064_caseD_13;
                    case 0x14:
                      goto switchD_0001d064_caseD_14;
                    case 0x15:
                      goto switchD_0001d064_caseD_15;
                    case 0x16:
                      goto switchD_0001d064_caseD_16;
                    case 0x17:
                      goto switchD_0001d064_caseD_17;
                    case 0x18:
                      goto switchD_0001d064_caseD_18;
                    case 0x19:
                      goto switchD_0001d064_caseD_19;
                    case 0x1a:
                      goto switchD_0001d064_caseD_1a;
                    case 0x1b:
                      goto switchD_0001d064_caseD_1b;
                    case 0x1c:
                      goto switchD_0001d064_caseD_1c;
                    case 0x1d:
                      goto switchD_0001d064_caseD_1d;
                    case 0x1e:
                      goto switchD_0001d064_caseD_1e;
                    case 0x1f:
                      goto switchD_0001d064_caseD_1f;
                    case 0x20:
                      goto switchD_0001d064_caseD_20;
                    case 0x21:
                      goto switchD_0001d064_caseD_21;
                    }
                  } while( true );
                case 5:
                  goto switchD_0001d064_caseD_5;
                case 6:
                  goto switchD_0001d064_caseD_6;
                case 7:
                  goto switchD_0001d064_caseD_7;
                case 8:
                  goto switchD_0001d064_caseD_8;
                case 9:
                  goto switchD_0001d064_caseD_9;
                case 10:
                  goto switchD_0001d064_caseD_a;
                case 0xb:
                  goto switchD_0001d064_caseD_b;
                case 0xc:
                  goto switchD_0001d064_caseD_c;
                case 0xd:
                  goto switchD_0001d064_caseD_d;
                case 0xe:
                  goto switchD_0001d064_caseD_e;
                case 0xf:
                  goto switchD_0001d064_caseD_f;
                case 0x10:
                  goto switchD_0001d064_caseD_10;
                case 0x11:
                  goto switchD_0001d064_caseD_11;
                case 0x12:
                  goto switchD_0001d064_caseD_12;
                case 0x13:
                  goto switchD_0001d064_caseD_13;
                case 0x14:
                  goto switchD_0001d064_caseD_14;
                case 0x15:
                  goto switchD_0001d064_caseD_15;
                case 0x16:
                  goto switchD_0001d064_caseD_16;
                case 0x17:
                  goto switchD_0001d064_caseD_17;
                case 0x18:
                  goto switchD_0001d064_caseD_18;
                case 0x19:
                  goto switchD_0001d064_caseD_19;
                case 0x1a:
                  goto switchD_0001d064_caseD_1a;
                case 0x1b:
                  goto switchD_0001d064_caseD_1b;
                case 0x1c:
                  goto switchD_0001d064_caseD_1c;
                case 0x1d:
                  goto switchD_0001d064_caseD_1d;
                case 0x1e:
                  goto switchD_0001d064_caseD_1e;
                case 0x1f:
                  goto switchD_0001d064_caseD_1f;
                case 0x20:
                  goto switchD_0001d064_caseD_20;
                case 0x21:
                  goto switchD_0001d064_caseD_21;
                }
              } while( true );
            case 4:
              goto switchD_0001d064_caseD_4;
            case 5:
              goto switchD_0001d064_caseD_5;
            case 6:
              goto switchD_0001d064_caseD_6;
            case 7:
              goto switchD_0001d064_caseD_7;
            case 8:
              goto switchD_0001d064_caseD_8;
            case 9:
              goto switchD_0001d064_caseD_9;
            case 10:
              goto switchD_0001d064_caseD_a;
            case 0xb:
              goto switchD_0001d064_caseD_b;
            case 0xc:
              goto switchD_0001d064_caseD_c;
            case 0xd:
              goto switchD_0001d064_caseD_d;
            case 0xe:
              goto switchD_0001d064_caseD_e;
            case 0xf:
              goto switchD_0001d064_caseD_f;
            case 0x10:
              goto switchD_0001d064_caseD_10;
            case 0x11:
              goto switchD_0001d064_caseD_11;
            case 0x12:
              goto switchD_0001d064_caseD_12;
            case 0x13:
              goto switchD_0001d064_caseD_13;
            case 0x14:
              goto switchD_0001d064_caseD_14;
            case 0x15:
              goto switchD_0001d064_caseD_15;
            case 0x16:
              goto switchD_0001d064_caseD_16;
            case 0x17:
              goto switchD_0001d064_caseD_17;
            case 0x18:
              goto switchD_0001d064_caseD_18;
            case 0x19:
              goto switchD_0001d064_caseD_19;
            case 0x1a:
              goto switchD_0001d064_caseD_1a;
            case 0x1b:
              goto switchD_0001d064_caseD_1b;
            case 0x1c:
              goto switchD_0001d064_caseD_1c;
            case 0x1d:
              goto switchD_0001d064_caseD_1d;
            case 0x1e:
              goto switchD_0001d064_caseD_1e;
            case 0x1f:
              goto switchD_0001d064_caseD_1f;
            case 0x20:
              goto switchD_0001d064_caseD_20;
            case 0x21:
              goto switchD_0001d064_caseD_21;
            }
          } while( true );
        case 3:
          goto switchD_0001d064_caseD_3;
        case 4:
          goto switchD_0001d064_caseD_4;
        case 5:
          goto switchD_0001d064_caseD_5;
        case 6:
          goto switchD_0001d064_caseD_6;
        case 7:
          goto switchD_0001d064_caseD_7;
        case 8:
          goto switchD_0001d064_caseD_8;
        case 9:
          goto switchD_0001d064_caseD_9;
        case 10:
          goto switchD_0001d064_caseD_a;
        case 0xb:
          goto switchD_0001d064_caseD_b;
        case 0xc:
          goto switchD_0001d064_caseD_c;
        case 0xd:
          goto switchD_0001d064_caseD_d;
        case 0xe:
          goto switchD_0001d064_caseD_e;
        case 0xf:
          goto switchD_0001d064_caseD_f;
        case 0x10:
          goto switchD_0001d064_caseD_10;
        case 0x11:
          goto switchD_0001d064_caseD_11;
        case 0x12:
          goto switchD_0001d064_caseD_12;
        case 0x13:
          goto switchD_0001d064_caseD_13;
        case 0x14:
          goto switchD_0001d064_caseD_14;
        case 0x15:
          goto switchD_0001d064_caseD_15;
        case 0x16:
          goto switchD_0001d064_caseD_16;
        case 0x17:
          goto switchD_0001d064_caseD_17;
        case 0x18:
          goto switchD_0001d064_caseD_18;
        case 0x19:
          goto switchD_0001d064_caseD_19;
        case 0x1a:
          goto switchD_0001d064_caseD_1a;
        case 0x1b:
          goto switchD_0001d064_caseD_1b;
        case 0x1c:
          goto switchD_0001d064_caseD_1c;
        case 0x1d:
          goto switchD_0001d064_caseD_1d;
        case 0x1e:
          goto switchD_0001d064_caseD_1e;
        case 0x1f:
          goto switchD_0001d064_caseD_1f;
        case 0x20:
          goto switchD_0001d064_caseD_20;
        case 0x21:
          goto switchD_0001d064_caseD_21;
        }
      } while( true );
    case 2:
      goto switchD_0001d064_caseD_2;
    case 3:
      goto switchD_0001d064_caseD_3;
    case 4:
      goto switchD_0001d064_caseD_4;
    case 5:
      goto switchD_0001d064_caseD_5;
    case 6:
      goto switchD_0001d064_caseD_6;
    case 7:
      goto switchD_0001d064_caseD_7;
    case 8:
      goto switchD_0001d064_caseD_8;
    case 9:
      goto switchD_0001d064_caseD_9;
    case 10:
      goto switchD_0001d064_caseD_a;
    case 0xb:
      goto switchD_0001d064_caseD_b;
    case 0xc:
      goto switchD_0001d064_caseD_c;
    case 0xd:
      goto switchD_0001d064_caseD_d;
    case 0xe:
      goto switchD_0001d064_caseD_e;
    case 0xf:
      goto switchD_0001d064_caseD_f;
    case 0x10:
      goto switchD_0001d064_caseD_10;
    case 0x11:
      goto switchD_0001d064_caseD_11;
    case 0x12:
      goto switchD_0001d064_caseD_12;
    case 0x13:
      goto switchD_0001d064_caseD_13;
    case 0x14:
      goto switchD_0001d064_caseD_14;
    case 0x15:
      goto switchD_0001d064_caseD_15;
    case 0x16:
      goto switchD_0001d064_caseD_16;
    case 0x17:
      goto switchD_0001d064_caseD_17;
    case 0x18:
      goto switchD_0001d064_caseD_18;
    case 0x19:
      goto switchD_0001d064_caseD_19;
    case 0x1a:
      goto switchD_0001d064_caseD_1a;
    case 0x1b:
      goto switchD_0001d064_caseD_1b;
    case 0x1c:
      goto switchD_0001d064_caseD_1c;
    case 0x1d:
      goto switchD_0001d064_caseD_1d;
    case 0x1e:
      goto switchD_0001d064_caseD_1e;
    case 0x1f:
      goto switchD_0001d064_caseD_1f;
    case 0x20:
      goto switchD_0001d064_caseD_20;
    case 0x21:
      goto switchD_0001d064_caseD_21;
    }
  } while( true );
}

