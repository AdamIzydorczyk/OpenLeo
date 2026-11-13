
undefined8 __regparm2
FUN_0001d58c(undefined4 param_1,undefined4 param_2,uint *param_3,uint *param_4,int param_5,
            int param_6)

{
  ushort uVar1;
  uint3 uVar2;
  undefined2 uVar3;
  uint uVar4;
  
  while (param_5 != 0) {
    uVar4 = *param_4;
    uVar1 = (ushort)uVar4;
    if (0x10 < (byte)uVar4) {
      param_5 = param_5 + -1;
      uVar1 = CONCAT11((char)(uVar4 >> 8),(byte)uVar4 - 0x11);
    }
    param_4 = (uint *)((int)param_4 + *(int *)(&DAT_000a8f48 + (uint)(uVar1 & 0xff) * 4));
    if ((char)uVar1 == '\x0f') {
      param_4 = param_4 + (uVar1 >> 8);
    }
  }
  uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
  switch(uVar4 & 0xff) {
  case 1:
    goto switchD_0001d5ca_caseD_1;
  case 2:
    goto switchD_0001d5ca_caseD_2;
  case 3:
    goto switchD_0001d5ca_caseD_3;
  case 4:
    goto switchD_0001d5ca_caseD_4;
  case 5:
    goto switchD_0001d5ca_caseD_5;
  case 6:
    goto switchD_0001d5ca_caseD_6;
  case 7:
    goto switchD_0001d5ca_caseD_7;
  case 8:
    goto switchD_0001d5ca_caseD_8;
  case 9:
    goto switchD_0001d5ca_caseD_9;
  case 10:
    goto switchD_0001d5ca_caseD_a;
  case 0xb:
    goto switchD_0001d5ca_caseD_b;
  case 0xc:
    goto switchD_0001d5ca_caseD_c;
  case 0xd:
    goto switchD_0001d5ca_caseD_d;
  case 0xe:
    goto switchD_0001d5ca_caseD_e;
  case 0xf:
    goto switchD_0001d5ca_caseD_f;
  case 0x10:
    goto switchD_0001d5ca_caseD_10;
  case 0x11:
    goto switchD_0001d5ca_caseD_11;
  case 0x12:
    goto switchD_0001d5ca_caseD_12;
  case 0x13:
    goto switchD_0001d5ca_caseD_13;
  case 0x14:
    goto switchD_0001d5ca_caseD_14;
  case 0x15:
    goto switchD_0001d5ca_caseD_15;
  case 0x16:
    goto switchD_0001d5ca_caseD_16;
  case 0x17:
    goto switchD_0001d5ca_caseD_17;
  case 0x18:
    goto switchD_0001d5ca_caseD_18;
  case 0x19:
    goto switchD_0001d5ca_caseD_19;
  case 0x1a:
    goto switchD_0001d5ca_caseD_1a;
  case 0x1b:
    goto switchD_0001d5ca_caseD_1b;
  case 0x1c:
    goto switchD_0001d5ca_caseD_1c;
  case 0x1d:
    goto switchD_0001d5ca_caseD_1d;
  case 0x1e:
    goto switchD_0001d5ca_caseD_1e;
  case 0x1f:
    goto switchD_0001d5ca_caseD_1f;
  case 0x20:
    goto switchD_0001d5ca_caseD_20;
  case 0x21:
    goto switchD_0001d5ca_caseD_21;
  }
switchD_0001d5ca_caseD_0:
  do {
    param_3 = param_3 + (uVar4 >> 8 & 0xff);
    param_4 = (uint *)((int)param_4 + 2);
    uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
    switch(uVar4 & 0xff) {
    case 1:
switchD_0001d5ca_caseD_1:
      do {
        *(char *)((int)param_3 + 3) = (char)(uVar4 >> 8);
        param_4 = (uint *)((int)param_4 + 2);
        param_3 = param_3 + 1;
        uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
        switch(uVar4 & 0xff) {
        case 0:
          goto switchD_0001d5ca_caseD_0;
        case 2:
switchD_0001d5ca_caseD_2:
          do {
            param_4 = (uint *)((int)param_4 + 2);
            *(char *)((int)param_3 + 2) = (char)(uVar4 >> 8);
            param_3 = param_3 + 1;
            uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
            switch(uVar4 & 0xff) {
            case 0:
              goto switchD_0001d5ca_caseD_0;
            case 1:
              goto switchD_0001d5ca_caseD_1;
            case 3:
switchD_0001d5ca_caseD_3:
              do {
                *(short *)((int)param_3 + 2) = (short)(uVar4 >> 0x10);
                param_4 = param_4 + 1;
                param_3 = param_3 + 1;
                uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                switch(uVar4 & 0xff) {
                case 0:
                  goto switchD_0001d5ca_caseD_0;
                case 1:
                  goto switchD_0001d5ca_caseD_1;
                case 2:
                  goto switchD_0001d5ca_caseD_2;
                case 4:
switchD_0001d5ca_caseD_4:
                  do {
                    param_4 = (uint *)((int)param_4 + 2);
                    *(char *)((int)param_3 + 1) = (char)(uVar4 >> 8);
                    param_3 = param_3 + 1;
                    uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                    switch(uVar4 & 0xff) {
                    case 0:
                      goto switchD_0001d5ca_caseD_0;
                    case 1:
                      goto switchD_0001d5ca_caseD_1;
                    case 2:
                      goto switchD_0001d5ca_caseD_2;
                    case 3:
                      goto switchD_0001d5ca_caseD_3;
                    case 5:
switchD_0001d5ca_caseD_5:
                      do {
                        *(char *)((int)param_3 + 1) = (char)(uVar4 >> 8);
                        *(char *)((int)param_3 + 3) = (char)(uVar4 >> 0x18);
                        param_4 = param_4 + 1;
                        param_3 = param_3 + 1;
                        uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                        switch(uVar4 & 0xff) {
                        case 0:
                          goto switchD_0001d5ca_caseD_0;
                        case 1:
                          goto switchD_0001d5ca_caseD_1;
                        case 2:
                          goto switchD_0001d5ca_caseD_2;
                        case 3:
                          goto switchD_0001d5ca_caseD_3;
                        case 4:
                          goto switchD_0001d5ca_caseD_4;
                        case 6:
switchD_0001d5ca_caseD_6:
                          do {
                            *(short *)((int)param_3 + 1) = (short)(uVar4 >> 8);
                            param_4 = param_4 + 1;
                            param_3 = param_3 + 1;
                            uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                            switch(uVar4 & 0xff) {
                            case 0:
                              goto switchD_0001d5ca_caseD_0;
                            case 1:
                              goto switchD_0001d5ca_caseD_1;
                            case 2:
                              goto switchD_0001d5ca_caseD_2;
                            case 3:
                              goto switchD_0001d5ca_caseD_3;
                            case 4:
                              goto switchD_0001d5ca_caseD_4;
                            case 5:
                              goto switchD_0001d5ca_caseD_5;
                            case 7:
switchD_0001d5ca_caseD_7:
                              do {
                                param_4 = param_4 + 1;
                                *param_3 = CONCAT31((int3)(uVar4 >> 8),(char)*param_3);
                                param_3 = param_3 + 1;
                                uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                switch(uVar4 & 0xff) {
                                case 0:
                                  goto switchD_0001d5ca_caseD_0;
                                case 1:
                                  goto switchD_0001d5ca_caseD_1;
                                case 2:
                                  goto switchD_0001d5ca_caseD_2;
                                case 3:
                                  goto switchD_0001d5ca_caseD_3;
                                case 4:
                                  goto switchD_0001d5ca_caseD_4;
                                case 5:
                                  goto switchD_0001d5ca_caseD_5;
                                case 6:
                                  goto switchD_0001d5ca_caseD_6;
                                case 8:
switchD_0001d5ca_caseD_8:
                                  do {
                                    param_4 = (uint *)((int)param_4 + 2);
                                    *(char *)param_3 = (char)(uVar4 >> 8);
                                    param_3 = param_3 + 1;
                                    uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                    switch(uVar4 & 0xff) {
                                    case 0:
                                      goto switchD_0001d5ca_caseD_0;
                                    case 1:
                                      goto switchD_0001d5ca_caseD_1;
                                    case 2:
                                      goto switchD_0001d5ca_caseD_2;
                                    case 3:
                                      goto switchD_0001d5ca_caseD_3;
                                    case 4:
                                      goto switchD_0001d5ca_caseD_4;
                                    case 5:
                                      goto switchD_0001d5ca_caseD_5;
                                    case 6:
                                      goto switchD_0001d5ca_caseD_6;
                                    case 7:
                                      goto switchD_0001d5ca_caseD_7;
                                    case 9:
switchD_0001d5ca_caseD_9:
                                      do {
                                        *(char *)param_3 = (char)(uVar4 >> 8);
                                        param_4 = param_4 + 1;
                                        *(char *)((int)param_3 + 3) = (char)(uVar4 >> 0x10);
                                        param_3 = param_3 + 1;
                                        uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                        switch(uVar4 & 0xff) {
                                        case 0:
                                          goto switchD_0001d5ca_caseD_0;
                                        case 1:
                                          goto switchD_0001d5ca_caseD_1;
                                        case 2:
                                          goto switchD_0001d5ca_caseD_2;
                                        case 3:
                                          goto switchD_0001d5ca_caseD_3;
                                        case 4:
                                          goto switchD_0001d5ca_caseD_4;
                                        case 5:
                                          goto switchD_0001d5ca_caseD_5;
                                        case 6:
                                          goto switchD_0001d5ca_caseD_6;
                                        case 7:
                                          goto switchD_0001d5ca_caseD_7;
                                        case 8:
                                          goto switchD_0001d5ca_caseD_8;
                                        case 10:
switchD_0001d5ca_caseD_a:
                                          do {
                                            *(char *)param_3 = (char)(uVar4 >> 8);
                                            param_4 = param_4 + 1;
                                            *(char *)((int)param_3 + 2) = (char)(uVar4 >> 0x18);
                                            param_3 = param_3 + 1;
                                            uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                            switch(uVar4 & 0xff) {
                                            case 0:
                                              goto switchD_0001d5ca_caseD_0;
                                            case 1:
                                              goto switchD_0001d5ca_caseD_1;
                                            case 2:
                                              goto switchD_0001d5ca_caseD_2;
                                            case 3:
                                              goto switchD_0001d5ca_caseD_3;
                                            case 4:
                                              goto switchD_0001d5ca_caseD_4;
                                            case 5:
                                              goto switchD_0001d5ca_caseD_5;
                                            case 6:
                                              goto switchD_0001d5ca_caseD_6;
                                            case 7:
                                              goto switchD_0001d5ca_caseD_7;
                                            case 8:
                                              goto switchD_0001d5ca_caseD_8;
                                            case 9:
                                              goto switchD_0001d5ca_caseD_9;
                                            case 0xb:
switchD_0001d5ca_caseD_b:
                                              do {
                                                param_4 = param_4 + 1;
                                                *param_3 = CONCAT22((short)(uVar4 >> 0x10),
                                                                    CONCAT11(*(undefined1 *)
                                                                              ((int)param_3 + 1),
                                                                             (char)(uVar4 >> 8)));
                                                param_3 = param_3 + 1;
                                                uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                switch(uVar4 & 0xff) {
                                                case 0:
                                                  goto switchD_0001d5ca_caseD_0;
                                                case 1:
                                                  goto switchD_0001d5ca_caseD_1;
                                                case 2:
                                                  goto switchD_0001d5ca_caseD_2;
                                                case 3:
                                                  goto switchD_0001d5ca_caseD_3;
                                                case 4:
                                                  goto switchD_0001d5ca_caseD_4;
                                                case 5:
                                                  goto switchD_0001d5ca_caseD_5;
                                                case 6:
                                                  goto switchD_0001d5ca_caseD_6;
                                                case 7:
                                                  goto switchD_0001d5ca_caseD_7;
                                                case 8:
                                                  goto switchD_0001d5ca_caseD_8;
                                                case 9:
                                                  goto switchD_0001d5ca_caseD_9;
                                                case 10:
                                                  goto switchD_0001d5ca_caseD_a;
                                                case 0xc:
switchD_0001d5ca_caseD_c:
                                                  do {
                                                    *(short *)param_3 = (short)(uVar4 >> 0x10);
                                                    param_4 = param_4 + 1;
                                                    param_3 = param_3 + 1;
                                                    uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                    switch(uVar4 & 0xff) {
                                                    case 0:
                                                      goto switchD_0001d5ca_caseD_0;
                                                    case 1:
                                                      goto switchD_0001d5ca_caseD_1;
                                                    case 2:
                                                      goto switchD_0001d5ca_caseD_2;
                                                    case 3:
                                                      goto switchD_0001d5ca_caseD_3;
                                                    case 4:
                                                      goto switchD_0001d5ca_caseD_4;
                                                    case 5:
                                                      goto switchD_0001d5ca_caseD_5;
                                                    case 6:
                                                      goto switchD_0001d5ca_caseD_6;
                                                    case 7:
                                                      goto switchD_0001d5ca_caseD_7;
                                                    case 8:
                                                      goto switchD_0001d5ca_caseD_8;
                                                    case 9:
                                                      goto switchD_0001d5ca_caseD_9;
                                                    case 10:
                                                      goto switchD_0001d5ca_caseD_a;
                                                    case 0xb:
                                                      goto switchD_0001d5ca_caseD_b;
                                                    case 0xd:
switchD_0001d5ca_caseD_d:
                                                      do {
                                                        uVar2 = (uint3)(uVar4 >> 8);
                                                        param_4 = param_4 + 1;
                                                        *param_3 = CONCAT31(uVar2,*(undefined1 *)
                                                                                   ((int)param_3 + 2
                                                                                   )) << 0x10 |
                                                                   (uint)(uVar2 >> 8);
                                                        param_3 = param_3 + 1;
                                                        uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                        switch(uVar4 & 0xff) {
                                                        case 0:
                                                          goto switchD_0001d5ca_caseD_0;
                                                        case 1:
                                                          goto switchD_0001d5ca_caseD_1;
                                                        case 2:
                                                          goto switchD_0001d5ca_caseD_2;
                                                        case 3:
                                                          goto switchD_0001d5ca_caseD_3;
                                                        case 4:
                                                          goto switchD_0001d5ca_caseD_4;
                                                        case 5:
                                                          goto switchD_0001d5ca_caseD_5;
                                                        case 6:
                                                          goto switchD_0001d5ca_caseD_6;
                                                        case 7:
                                                          goto switchD_0001d5ca_caseD_7;
                                                        case 8:
                                                          goto switchD_0001d5ca_caseD_8;
                                                        case 9:
                                                          goto switchD_0001d5ca_caseD_9;
                                                        case 10:
                                                          goto switchD_0001d5ca_caseD_a;
                                                        case 0xb:
                                                          goto switchD_0001d5ca_caseD_b;
                                                        case 0xc:
                                                          goto switchD_0001d5ca_caseD_c;
                                                        case 0xe:
switchD_0001d5ca_caseD_e:
                                                          do {
                                                            uVar2 = (uint3)(uVar4 >> 8);
                                                            param_4 = param_4 + 1;
                                                            *param_3 = (uint)uVar2 |
                                                                       CONCAT31(uVar2,*(undefined1 *
                                                                                       )((int)
                                                  param_3 + 3)) << 0x18;
                                                  param_3 = param_3 + 1;
                                                  uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                  switch(uVar4 & 0xff) {
                                                  case 0:
                                                    goto switchD_0001d5ca_caseD_0;
                                                  case 1:
                                                    goto switchD_0001d5ca_caseD_1;
                                                  case 2:
                                                    goto switchD_0001d5ca_caseD_2;
                                                  case 3:
                                                    goto switchD_0001d5ca_caseD_3;
                                                  case 4:
                                                    goto switchD_0001d5ca_caseD_4;
                                                  case 5:
                                                    goto switchD_0001d5ca_caseD_5;
                                                  case 6:
                                                    goto switchD_0001d5ca_caseD_6;
                                                  case 7:
                                                    goto switchD_0001d5ca_caseD_7;
                                                  case 8:
                                                    goto switchD_0001d5ca_caseD_8;
                                                  case 9:
                                                    goto switchD_0001d5ca_caseD_9;
                                                  case 10:
                                                    goto switchD_0001d5ca_caseD_a;
                                                  case 0xb:
                                                    goto switchD_0001d5ca_caseD_b;
                                                  case 0xc:
                                                    goto switchD_0001d5ca_caseD_c;
                                                  case 0xd:
                                                    goto switchD_0001d5ca_caseD_d;
                                                  case 0xf:
switchD_0001d5ca_caseD_f:
                                                    do {
                                                      param_4 = (uint *)((int)param_4 + 2);
                                                      for (uVar4 = uVar4 >> 8 & 0xff; uVar4 != 0;
                                                          uVar4 = uVar4 - 1) {
                                                        *param_3 = *param_4;
                                                        param_4 = param_4 + 1;
                                                        param_3 = param_3 + 1;
                                                      }
                                                      uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                      switch(uVar4 & 0xff) {
                                                      case 0:
                                                        goto switchD_0001d5ca_caseD_0;
                                                      case 1:
                                                        goto switchD_0001d5ca_caseD_1;
                                                      case 2:
                                                        goto switchD_0001d5ca_caseD_2;
                                                      case 3:
                                                        goto switchD_0001d5ca_caseD_3;
                                                      case 4:
                                                        goto switchD_0001d5ca_caseD_4;
                                                      case 5:
                                                        goto switchD_0001d5ca_caseD_5;
                                                      case 6:
                                                        goto switchD_0001d5ca_caseD_6;
                                                      case 7:
                                                        goto switchD_0001d5ca_caseD_7;
                                                      case 8:
                                                        goto switchD_0001d5ca_caseD_8;
                                                      case 9:
                                                        goto switchD_0001d5ca_caseD_9;
                                                      case 10:
                                                        goto switchD_0001d5ca_caseD_a;
                                                      case 0xb:
                                                        goto switchD_0001d5ca_caseD_b;
                                                      case 0xc:
                                                        goto switchD_0001d5ca_caseD_c;
                                                      case 0xd:
                                                        goto switchD_0001d5ca_caseD_d;
                                                      case 0xe:
                                                        goto switchD_0001d5ca_caseD_e;
                                                      case 0x10:
switchD_0001d5ca_caseD_10:
                                                        do {
                                                          uVar3 = (undefined2)(uVar4 >> 0x10);
                                                          for (uVar4 = uVar4 >> 8 & 0xff; uVar4 != 0
                                                              ; uVar4 = uVar4 - 1) {
                                                            *param_3 = CONCAT22(uVar3,uVar3);
                                                            param_3 = param_3 + 1;
                                                          }
                                                          param_4 = param_4 + 1;
                                                          uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                          switch(uVar4 & 0xff) {
                                                          case 0:
                                                            goto switchD_0001d5ca_caseD_0;
                                                          case 1:
                                                            goto switchD_0001d5ca_caseD_1;
                                                          case 2:
                                                            goto switchD_0001d5ca_caseD_2;
                                                          case 3:
                                                            goto switchD_0001d5ca_caseD_3;
                                                          case 4:
                                                            goto switchD_0001d5ca_caseD_4;
                                                          case 5:
                                                            goto switchD_0001d5ca_caseD_5;
                                                          case 6:
                                                            goto switchD_0001d5ca_caseD_6;
                                                          case 7:
                                                            goto switchD_0001d5ca_caseD_7;
                                                          case 8:
                                                            goto switchD_0001d5ca_caseD_8;
                                                          case 9:
                                                            goto switchD_0001d5ca_caseD_9;
                                                          case 10:
                                                            goto switchD_0001d5ca_caseD_a;
                                                          case 0xb:
                                                            goto switchD_0001d5ca_caseD_b;
                                                          case 0xc:
                                                            goto switchD_0001d5ca_caseD_c;
                                                          case 0xd:
                                                            goto switchD_0001d5ca_caseD_d;
                                                          case 0xe:
                                                            goto switchD_0001d5ca_caseD_e;
                                                          case 0xf:
                                                            goto switchD_0001d5ca_caseD_f;
                                                          case 0x11:
switchD_0001d5ca_caseD_11:
                                                            do {
                                                              param_4 = (uint *)((int)param_4 + 2);
                                                              param_6 = param_6 + -1;
                                                              if (param_6 == 0) {
LAB_0001d5d1:
                                                                return CONCAT44(param_2,param_1);
                                                              }
                                                              param_3 = param_3 + (uVar4 >> 8 & 0xff
                                                                                  ) + 0x50;
                                                              uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                              switch(uVar4 & 0xff) {
                                                              case 0:
                                                                goto switchD_0001d5ca_caseD_0;
                                                              case 1:
                                                                goto switchD_0001d5ca_caseD_1;
                                                              case 2:
                                                                goto switchD_0001d5ca_caseD_2;
                                                              case 3:
                                                                goto switchD_0001d5ca_caseD_3;
                                                              case 4:
                                                                goto switchD_0001d5ca_caseD_4;
                                                              case 5:
                                                                goto switchD_0001d5ca_caseD_5;
                                                              case 6:
                                                                goto switchD_0001d5ca_caseD_6;
                                                              case 7:
                                                                goto switchD_0001d5ca_caseD_7;
                                                              case 8:
                                                                goto switchD_0001d5ca_caseD_8;
                                                              case 9:
                                                                goto switchD_0001d5ca_caseD_9;
                                                              case 10:
                                                                goto switchD_0001d5ca_caseD_a;
                                                              case 0xb:
                                                                goto switchD_0001d5ca_caseD_b;
                                                              case 0xc:
                                                                goto switchD_0001d5ca_caseD_c;
                                                              case 0xd:
                                                                goto switchD_0001d5ca_caseD_d;
                                                              case 0xe:
                                                                goto switchD_0001d5ca_caseD_e;
                                                              case 0xf:
                                                                goto switchD_0001d5ca_caseD_f;
                                                              case 0x10:
                                                                goto switchD_0001d5ca_caseD_10;
                                                              case 0x12:
switchD_0001d5ca_caseD_12:
                                                                do {
                                                                  *(char *)((int)param_3 + 3) =
                                                                       (char)(uVar4 >> 8);
                                                                  param_4 = (uint *)((int)param_4 +
                                                                                    2);
                                                                  param_6 = param_6 + -1;
                                                                  if (param_6 == 0)
                                                                  goto LAB_0001d5d1;
                                                                  param_3 = param_3 + 0x51;
                                                                  uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                                  switch(uVar4 & 0xff) {
                                                                  case 0:
                                                                    goto switchD_0001d5ca_caseD_0;
                                                                  case 1:
                                                                    goto switchD_0001d5ca_caseD_1;
                                                                  case 2:
                                                                    goto switchD_0001d5ca_caseD_2;
                                                                  case 3:
                                                                    goto switchD_0001d5ca_caseD_3;
                                                                  case 4:
                                                                    goto switchD_0001d5ca_caseD_4;
                                                                  case 5:
                                                                    goto switchD_0001d5ca_caseD_5;
                                                                  case 6:
                                                                    goto switchD_0001d5ca_caseD_6;
                                                                  case 7:
                                                                    goto switchD_0001d5ca_caseD_7;
                                                                  case 8:
                                                                    goto switchD_0001d5ca_caseD_8;
                                                                  case 9:
                                                                    goto switchD_0001d5ca_caseD_9;
                                                                  case 10:
                                                                    goto switchD_0001d5ca_caseD_a;
                                                                  case 0xb:
                                                                    goto switchD_0001d5ca_caseD_b;
                                                                  case 0xc:
                                                                    goto switchD_0001d5ca_caseD_c;
                                                                  case 0xd:
                                                                    goto switchD_0001d5ca_caseD_d;
                                                                  case 0xe:
                                                                    goto switchD_0001d5ca_caseD_e;
                                                                  case 0xf:
                                                                    goto switchD_0001d5ca_caseD_f;
                                                                  case 0x10:
                                                                    goto switchD_0001d5ca_caseD_10;
                                                                  case 0x11:
                                                                    goto switchD_0001d5ca_caseD_11;
                                                                  case 0x13:
switchD_0001d5ca_caseD_13:
                                                                    do {
                                                                      param_4 = (uint *)((int)
                                                  param_4 + 2);
                                                  *(char *)((int)param_3 + 2) = (char)(uVar4 >> 8);
                                                  param_6 = param_6 + -1;
                                                  if (param_6 == 0) goto LAB_0001d5d1;
                                                  param_3 = param_3 + 0x51;
                                                  uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                  switch(uVar4 & 0xff) {
                                                  case 0:
                                                    goto switchD_0001d5ca_caseD_0;
                                                  case 1:
                                                    goto switchD_0001d5ca_caseD_1;
                                                  case 2:
                                                    goto switchD_0001d5ca_caseD_2;
                                                  case 3:
                                                    goto switchD_0001d5ca_caseD_3;
                                                  case 4:
                                                    goto switchD_0001d5ca_caseD_4;
                                                  case 5:
                                                    goto switchD_0001d5ca_caseD_5;
                                                  case 6:
                                                    goto switchD_0001d5ca_caseD_6;
                                                  case 7:
                                                    goto switchD_0001d5ca_caseD_7;
                                                  case 8:
                                                    goto switchD_0001d5ca_caseD_8;
                                                  case 9:
                                                    goto switchD_0001d5ca_caseD_9;
                                                  case 10:
                                                    goto switchD_0001d5ca_caseD_a;
                                                  case 0xb:
                                                    goto switchD_0001d5ca_caseD_b;
                                                  case 0xc:
                                                    goto switchD_0001d5ca_caseD_c;
                                                  case 0xd:
                                                    goto switchD_0001d5ca_caseD_d;
                                                  case 0xe:
                                                    goto switchD_0001d5ca_caseD_e;
                                                  case 0xf:
                                                    goto switchD_0001d5ca_caseD_f;
                                                  case 0x10:
                                                    goto switchD_0001d5ca_caseD_10;
                                                  case 0x11:
                                                    goto switchD_0001d5ca_caseD_11;
                                                  case 0x12:
                                                    goto switchD_0001d5ca_caseD_12;
                                                  case 0x14:
switchD_0001d5ca_caseD_14:
                                                    do {
                                                      *(short *)((int)param_3 + 2) =
                                                           (short)(uVar4 >> 0x10);
                                                      param_4 = param_4 + 1;
                                                      param_6 = param_6 + -1;
                                                      if (param_6 == 0) goto LAB_0001d5d1;
                                                      param_3 = param_3 + 0x51;
                                                      uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                      switch(uVar4 & 0xff) {
                                                      case 0:
                                                        goto switchD_0001d5ca_caseD_0;
                                                      case 1:
                                                        goto switchD_0001d5ca_caseD_1;
                                                      case 2:
                                                        goto switchD_0001d5ca_caseD_2;
                                                      case 3:
                                                        goto switchD_0001d5ca_caseD_3;
                                                      case 4:
                                                        goto switchD_0001d5ca_caseD_4;
                                                      case 5:
                                                        goto switchD_0001d5ca_caseD_5;
                                                      case 6:
                                                        goto switchD_0001d5ca_caseD_6;
                                                      case 7:
                                                        goto switchD_0001d5ca_caseD_7;
                                                      case 8:
                                                        goto switchD_0001d5ca_caseD_8;
                                                      case 9:
                                                        goto switchD_0001d5ca_caseD_9;
                                                      case 10:
                                                        goto switchD_0001d5ca_caseD_a;
                                                      case 0xb:
                                                        goto switchD_0001d5ca_caseD_b;
                                                      case 0xc:
                                                        goto switchD_0001d5ca_caseD_c;
                                                      case 0xd:
                                                        goto switchD_0001d5ca_caseD_d;
                                                      case 0xe:
                                                        goto switchD_0001d5ca_caseD_e;
                                                      case 0xf:
                                                        goto switchD_0001d5ca_caseD_f;
                                                      case 0x10:
                                                        goto switchD_0001d5ca_caseD_10;
                                                      case 0x11:
                                                        goto switchD_0001d5ca_caseD_11;
                                                      case 0x12:
                                                        goto switchD_0001d5ca_caseD_12;
                                                      case 0x13:
                                                        goto switchD_0001d5ca_caseD_13;
                                                      case 0x15:
switchD_0001d5ca_caseD_15:
                                                        do {
                                                          param_4 = (uint *)((int)param_4 + 2);
                                                          *(char *)((int)param_3 + 1) =
                                                               (char)(uVar4 >> 8);
                                                          param_6 = param_6 + -1;
                                                          if (param_6 == 0) goto LAB_0001d5d1;
                                                          param_3 = param_3 + 0x51;
                                                          uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                          switch(uVar4 & 0xff) {
                                                          case 0:
                                                            goto switchD_0001d5ca_caseD_0;
                                                          case 1:
                                                            goto switchD_0001d5ca_caseD_1;
                                                          case 2:
                                                            goto switchD_0001d5ca_caseD_2;
                                                          case 3:
                                                            goto switchD_0001d5ca_caseD_3;
                                                          case 4:
                                                            goto switchD_0001d5ca_caseD_4;
                                                          case 5:
                                                            goto switchD_0001d5ca_caseD_5;
                                                          case 6:
                                                            goto switchD_0001d5ca_caseD_6;
                                                          case 7:
                                                            goto switchD_0001d5ca_caseD_7;
                                                          case 8:
                                                            goto switchD_0001d5ca_caseD_8;
                                                          case 9:
                                                            goto switchD_0001d5ca_caseD_9;
                                                          case 10:
                                                            goto switchD_0001d5ca_caseD_a;
                                                          case 0xb:
                                                            goto switchD_0001d5ca_caseD_b;
                                                          case 0xc:
                                                            goto switchD_0001d5ca_caseD_c;
                                                          case 0xd:
                                                            goto switchD_0001d5ca_caseD_d;
                                                          case 0xe:
                                                            goto switchD_0001d5ca_caseD_e;
                                                          case 0xf:
                                                            goto switchD_0001d5ca_caseD_f;
                                                          case 0x10:
                                                            goto switchD_0001d5ca_caseD_10;
                                                          case 0x11:
                                                            goto switchD_0001d5ca_caseD_11;
                                                          case 0x12:
                                                            goto switchD_0001d5ca_caseD_12;
                                                          case 0x13:
                                                            goto switchD_0001d5ca_caseD_13;
                                                          case 0x14:
                                                            goto switchD_0001d5ca_caseD_14;
                                                          case 0x16:
switchD_0001d5ca_caseD_16:
                                                            do {
                                                              *(char *)((int)param_3 + 1) =
                                                                   (char)(uVar4 >> 8);
                                                              *(char *)((int)param_3 + 3) =
                                                                   (char)(uVar4 >> 0x18);
                                                              param_4 = param_4 + 1;
                                                              param_6 = param_6 + -1;
                                                              if (param_6 == 0) goto LAB_0001d5d1;
                                                              param_3 = param_3 + 0x51;
                                                              uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                              switch(uVar4 & 0xff) {
                                                              case 0:
                                                                goto switchD_0001d5ca_caseD_0;
                                                              case 1:
                                                                goto switchD_0001d5ca_caseD_1;
                                                              case 2:
                                                                goto switchD_0001d5ca_caseD_2;
                                                              case 3:
                                                                goto switchD_0001d5ca_caseD_3;
                                                              case 4:
                                                                goto switchD_0001d5ca_caseD_4;
                                                              case 5:
                                                                goto switchD_0001d5ca_caseD_5;
                                                              case 6:
                                                                goto switchD_0001d5ca_caseD_6;
                                                              case 7:
                                                                goto switchD_0001d5ca_caseD_7;
                                                              case 8:
                                                                goto switchD_0001d5ca_caseD_8;
                                                              case 9:
                                                                goto switchD_0001d5ca_caseD_9;
                                                              case 10:
                                                                goto switchD_0001d5ca_caseD_a;
                                                              case 0xb:
                                                                goto switchD_0001d5ca_caseD_b;
                                                              case 0xc:
                                                                goto switchD_0001d5ca_caseD_c;
                                                              case 0xd:
                                                                goto switchD_0001d5ca_caseD_d;
                                                              case 0xe:
                                                                goto switchD_0001d5ca_caseD_e;
                                                              case 0xf:
                                                                goto switchD_0001d5ca_caseD_f;
                                                              case 0x10:
                                                                goto switchD_0001d5ca_caseD_10;
                                                              case 0x11:
                                                                goto switchD_0001d5ca_caseD_11;
                                                              case 0x12:
                                                                goto switchD_0001d5ca_caseD_12;
                                                              case 0x13:
                                                                goto switchD_0001d5ca_caseD_13;
                                                              case 0x14:
                                                                goto switchD_0001d5ca_caseD_14;
                                                              case 0x15:
                                                                goto switchD_0001d5ca_caseD_15;
                                                              case 0x17:
switchD_0001d5ca_caseD_17:
                                                                do {
                                                                  *(short *)((int)param_3 + 1) =
                                                                       (short)(uVar4 >> 8);
                                                                  param_4 = param_4 + 1;
                                                                  param_6 = param_6 + -1;
                                                                  if (param_6 == 0)
                                                                  goto LAB_0001d5d1;
                                                                  param_3 = param_3 + 0x51;
                                                                  uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                                  switch(uVar4 & 0xff) {
                                                                  case 0:
                                                                    goto switchD_0001d5ca_caseD_0;
                                                                  case 1:
                                                                    goto switchD_0001d5ca_caseD_1;
                                                                  case 2:
                                                                    goto switchD_0001d5ca_caseD_2;
                                                                  case 3:
                                                                    goto switchD_0001d5ca_caseD_3;
                                                                  case 4:
                                                                    goto switchD_0001d5ca_caseD_4;
                                                                  case 5:
                                                                    goto switchD_0001d5ca_caseD_5;
                                                                  case 6:
                                                                    goto switchD_0001d5ca_caseD_6;
                                                                  case 7:
                                                                    goto switchD_0001d5ca_caseD_7;
                                                                  case 8:
                                                                    goto switchD_0001d5ca_caseD_8;
                                                                  case 9:
                                                                    goto switchD_0001d5ca_caseD_9;
                                                                  case 10:
                                                                    goto switchD_0001d5ca_caseD_a;
                                                                  case 0xb:
                                                                    goto switchD_0001d5ca_caseD_b;
                                                                  case 0xc:
                                                                    goto switchD_0001d5ca_caseD_c;
                                                                  case 0xd:
                                                                    goto switchD_0001d5ca_caseD_d;
                                                                  case 0xe:
                                                                    goto switchD_0001d5ca_caseD_e;
                                                                  case 0xf:
                                                                    goto switchD_0001d5ca_caseD_f;
                                                                  case 0x10:
                                                                    goto switchD_0001d5ca_caseD_10;
                                                                  case 0x11:
                                                                    goto switchD_0001d5ca_caseD_11;
                                                                  case 0x12:
                                                                    goto switchD_0001d5ca_caseD_12;
                                                                  case 0x13:
                                                                    goto switchD_0001d5ca_caseD_13;
                                                                  case 0x14:
                                                                    goto switchD_0001d5ca_caseD_14;
                                                                  case 0x15:
                                                                    goto switchD_0001d5ca_caseD_15;
                                                                  case 0x16:
                                                                    goto switchD_0001d5ca_caseD_16;
                                                                  case 0x18:
switchD_0001d5ca_caseD_18:
                                                                    do {
                                                                      param_4 = param_4 + 1;
                                                                      *param_3 = CONCAT31((int3)(
                                                  uVar4 >> 8),(char)*param_3);
                                                  param_6 = param_6 + -1;
                                                  if (param_6 == 0) goto LAB_0001d5d1;
                                                  param_3 = param_3 + 0x51;
                                                  uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                  switch(uVar4 & 0xff) {
                                                  case 0:
                                                    goto switchD_0001d5ca_caseD_0;
                                                  case 1:
                                                    goto switchD_0001d5ca_caseD_1;
                                                  case 2:
                                                    goto switchD_0001d5ca_caseD_2;
                                                  case 3:
                                                    goto switchD_0001d5ca_caseD_3;
                                                  case 4:
                                                    goto switchD_0001d5ca_caseD_4;
                                                  case 5:
                                                    goto switchD_0001d5ca_caseD_5;
                                                  case 6:
                                                    goto switchD_0001d5ca_caseD_6;
                                                  case 7:
                                                    goto switchD_0001d5ca_caseD_7;
                                                  case 8:
                                                    goto switchD_0001d5ca_caseD_8;
                                                  case 9:
                                                    goto switchD_0001d5ca_caseD_9;
                                                  case 10:
                                                    goto switchD_0001d5ca_caseD_a;
                                                  case 0xb:
                                                    goto switchD_0001d5ca_caseD_b;
                                                  case 0xc:
                                                    goto switchD_0001d5ca_caseD_c;
                                                  case 0xd:
                                                    goto switchD_0001d5ca_caseD_d;
                                                  case 0xe:
                                                    goto switchD_0001d5ca_caseD_e;
                                                  case 0xf:
                                                    goto switchD_0001d5ca_caseD_f;
                                                  case 0x10:
                                                    goto switchD_0001d5ca_caseD_10;
                                                  case 0x11:
                                                    goto switchD_0001d5ca_caseD_11;
                                                  case 0x12:
                                                    goto switchD_0001d5ca_caseD_12;
                                                  case 0x13:
                                                    goto switchD_0001d5ca_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d5ca_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d5ca_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d5ca_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d5ca_caseD_17;
                                                  case 0x19:
switchD_0001d5ca_caseD_19:
                                                    do {
                                                      param_4 = (uint *)((int)param_4 + 2);
                                                      *(char *)param_3 = (char)(uVar4 >> 8);
                                                      param_6 = param_6 + -1;
                                                      if (param_6 == 0) goto LAB_0001d5d1;
                                                      param_3 = param_3 + 0x51;
                                                      uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                      switch(uVar4 & 0xff) {
                                                      case 0:
                                                        goto switchD_0001d5ca_caseD_0;
                                                      case 1:
                                                        goto switchD_0001d5ca_caseD_1;
                                                      case 2:
                                                        goto switchD_0001d5ca_caseD_2;
                                                      case 3:
                                                        goto switchD_0001d5ca_caseD_3;
                                                      case 4:
                                                        goto switchD_0001d5ca_caseD_4;
                                                      case 5:
                                                        goto switchD_0001d5ca_caseD_5;
                                                      case 6:
                                                        goto switchD_0001d5ca_caseD_6;
                                                      case 7:
                                                        goto switchD_0001d5ca_caseD_7;
                                                      case 8:
                                                        goto switchD_0001d5ca_caseD_8;
                                                      case 9:
                                                        goto switchD_0001d5ca_caseD_9;
                                                      case 10:
                                                        goto switchD_0001d5ca_caseD_a;
                                                      case 0xb:
                                                        goto switchD_0001d5ca_caseD_b;
                                                      case 0xc:
                                                        goto switchD_0001d5ca_caseD_c;
                                                      case 0xd:
                                                        goto switchD_0001d5ca_caseD_d;
                                                      case 0xe:
                                                        goto switchD_0001d5ca_caseD_e;
                                                      case 0xf:
                                                        goto switchD_0001d5ca_caseD_f;
                                                      case 0x10:
                                                        goto switchD_0001d5ca_caseD_10;
                                                      case 0x11:
                                                        goto switchD_0001d5ca_caseD_11;
                                                      case 0x12:
                                                        goto switchD_0001d5ca_caseD_12;
                                                      case 0x13:
                                                        goto switchD_0001d5ca_caseD_13;
                                                      case 0x14:
                                                        goto switchD_0001d5ca_caseD_14;
                                                      case 0x15:
                                                        goto switchD_0001d5ca_caseD_15;
                                                      case 0x16:
                                                        goto switchD_0001d5ca_caseD_16;
                                                      case 0x17:
                                                        goto switchD_0001d5ca_caseD_17;
                                                      case 0x18:
                                                        goto switchD_0001d5ca_caseD_18;
                                                      case 0x1a:
switchD_0001d5ca_caseD_1a:
                                                        do {
                                                          *(char *)param_3 = (char)(uVar4 >> 8);
                                                          param_4 = param_4 + 1;
                                                          *(char *)((int)param_3 + 3) =
                                                               (char)(uVar4 >> 0x10);
                                                          param_6 = param_6 + -1;
                                                          if (param_6 == 0) goto LAB_0001d5d1;
                                                          param_3 = param_3 + 0x51;
                                                          uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                          switch(uVar4 & 0xff) {
                                                          case 0:
                                                            goto switchD_0001d5ca_caseD_0;
                                                          case 1:
                                                            goto switchD_0001d5ca_caseD_1;
                                                          case 2:
                                                            goto switchD_0001d5ca_caseD_2;
                                                          case 3:
                                                            goto switchD_0001d5ca_caseD_3;
                                                          case 4:
                                                            goto switchD_0001d5ca_caseD_4;
                                                          case 5:
                                                            goto switchD_0001d5ca_caseD_5;
                                                          case 6:
                                                            goto switchD_0001d5ca_caseD_6;
                                                          case 7:
                                                            goto switchD_0001d5ca_caseD_7;
                                                          case 8:
                                                            goto switchD_0001d5ca_caseD_8;
                                                          case 9:
                                                            goto switchD_0001d5ca_caseD_9;
                                                          case 10:
                                                            goto switchD_0001d5ca_caseD_a;
                                                          case 0xb:
                                                            goto switchD_0001d5ca_caseD_b;
                                                          case 0xc:
                                                            goto switchD_0001d5ca_caseD_c;
                                                          case 0xd:
                                                            goto switchD_0001d5ca_caseD_d;
                                                          case 0xe:
                                                            goto switchD_0001d5ca_caseD_e;
                                                          case 0xf:
                                                            goto switchD_0001d5ca_caseD_f;
                                                          case 0x10:
                                                            goto switchD_0001d5ca_caseD_10;
                                                          case 0x11:
                                                            goto switchD_0001d5ca_caseD_11;
                                                          case 0x12:
                                                            goto switchD_0001d5ca_caseD_12;
                                                          case 0x13:
                                                            goto switchD_0001d5ca_caseD_13;
                                                          case 0x14:
                                                            goto switchD_0001d5ca_caseD_14;
                                                          case 0x15:
                                                            goto switchD_0001d5ca_caseD_15;
                                                          case 0x16:
                                                            goto switchD_0001d5ca_caseD_16;
                                                          case 0x17:
                                                            goto switchD_0001d5ca_caseD_17;
                                                          case 0x18:
                                                            goto switchD_0001d5ca_caseD_18;
                                                          case 0x19:
                                                            goto switchD_0001d5ca_caseD_19;
                                                          case 0x1b:
switchD_0001d5ca_caseD_1b:
                                                            do {
                                                              *(char *)param_3 = (char)(uVar4 >> 8);
                                                              param_4 = param_4 + 1;
                                                              *(char *)((int)param_3 + 2) =
                                                                   (char)(uVar4 >> 0x18);
                                                              param_6 = param_6 + -1;
                                                              if (param_6 == 0) goto LAB_0001d5d1;
                                                              param_3 = param_3 + 0x51;
                                                              uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                              switch(uVar4 & 0xff) {
                                                              case 0:
                                                                goto switchD_0001d5ca_caseD_0;
                                                              case 1:
                                                                goto switchD_0001d5ca_caseD_1;
                                                              case 2:
                                                                goto switchD_0001d5ca_caseD_2;
                                                              case 3:
                                                                goto switchD_0001d5ca_caseD_3;
                                                              case 4:
                                                                goto switchD_0001d5ca_caseD_4;
                                                              case 5:
                                                                goto switchD_0001d5ca_caseD_5;
                                                              case 6:
                                                                goto switchD_0001d5ca_caseD_6;
                                                              case 7:
                                                                goto switchD_0001d5ca_caseD_7;
                                                              case 8:
                                                                goto switchD_0001d5ca_caseD_8;
                                                              case 9:
                                                                goto switchD_0001d5ca_caseD_9;
                                                              case 10:
                                                                goto switchD_0001d5ca_caseD_a;
                                                              case 0xb:
                                                                goto switchD_0001d5ca_caseD_b;
                                                              case 0xc:
                                                                goto switchD_0001d5ca_caseD_c;
                                                              case 0xd:
                                                                goto switchD_0001d5ca_caseD_d;
                                                              case 0xe:
                                                                goto switchD_0001d5ca_caseD_e;
                                                              case 0xf:
                                                                goto switchD_0001d5ca_caseD_f;
                                                              case 0x10:
                                                                goto switchD_0001d5ca_caseD_10;
                                                              case 0x11:
                                                                goto switchD_0001d5ca_caseD_11;
                                                              case 0x12:
                                                                goto switchD_0001d5ca_caseD_12;
                                                              case 0x13:
                                                                goto switchD_0001d5ca_caseD_13;
                                                              case 0x14:
                                                                goto switchD_0001d5ca_caseD_14;
                                                              case 0x15:
                                                                goto switchD_0001d5ca_caseD_15;
                                                              case 0x16:
                                                                goto switchD_0001d5ca_caseD_16;
                                                              case 0x17:
                                                                goto switchD_0001d5ca_caseD_17;
                                                              case 0x18:
                                                                goto switchD_0001d5ca_caseD_18;
                                                              case 0x19:
                                                                goto switchD_0001d5ca_caseD_19;
                                                              case 0x1a:
                                                                goto switchD_0001d5ca_caseD_1a;
                                                              case 0x1c:
switchD_0001d5ca_caseD_1c:
                                                                do {
                                                                  param_4 = param_4 + 1;
                                                                  *param_3 = CONCAT22((short)(uVar4 
                                                  >> 0x10),CONCAT11(*(undefined1 *)
                                                                     ((int)param_3 + 1),
                                                                    (char)(uVar4 >> 8)));
                                                  param_6 = param_6 + -1;
                                                  if (param_6 == 0) goto LAB_0001d5d1;
                                                  param_3 = param_3 + 0x51;
                                                  uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                  switch(uVar4 & 0xff) {
                                                  case 0:
                                                    goto switchD_0001d5ca_caseD_0;
                                                  case 1:
                                                    goto switchD_0001d5ca_caseD_1;
                                                  case 2:
                                                    goto switchD_0001d5ca_caseD_2;
                                                  case 3:
                                                    goto switchD_0001d5ca_caseD_3;
                                                  case 4:
                                                    goto switchD_0001d5ca_caseD_4;
                                                  case 5:
                                                    goto switchD_0001d5ca_caseD_5;
                                                  case 6:
                                                    goto switchD_0001d5ca_caseD_6;
                                                  case 7:
                                                    goto switchD_0001d5ca_caseD_7;
                                                  case 8:
                                                    goto switchD_0001d5ca_caseD_8;
                                                  case 9:
                                                    goto switchD_0001d5ca_caseD_9;
                                                  case 10:
                                                    goto switchD_0001d5ca_caseD_a;
                                                  case 0xb:
                                                    goto switchD_0001d5ca_caseD_b;
                                                  case 0xc:
                                                    goto switchD_0001d5ca_caseD_c;
                                                  case 0xd:
                                                    goto switchD_0001d5ca_caseD_d;
                                                  case 0xe:
                                                    goto switchD_0001d5ca_caseD_e;
                                                  case 0xf:
                                                    goto switchD_0001d5ca_caseD_f;
                                                  case 0x10:
                                                    goto switchD_0001d5ca_caseD_10;
                                                  case 0x11:
                                                    goto switchD_0001d5ca_caseD_11;
                                                  case 0x12:
                                                    goto switchD_0001d5ca_caseD_12;
                                                  case 0x13:
                                                    goto switchD_0001d5ca_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d5ca_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d5ca_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d5ca_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d5ca_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d5ca_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d5ca_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d5ca_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d5ca_caseD_1b;
                                                  case 0x1d:
switchD_0001d5ca_caseD_1d:
                                                    do {
                                                      *(short *)param_3 = (short)(uVar4 >> 0x10);
                                                      param_4 = param_4 + 1;
                                                      param_6 = param_6 + -1;
                                                      if (param_6 == 0) goto LAB_0001d5d1;
                                                      param_3 = param_3 + 0x51;
                                                      uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                      switch(uVar4 & 0xff) {
                                                      case 0:
                                                        goto switchD_0001d5ca_caseD_0;
                                                      case 1:
                                                        goto switchD_0001d5ca_caseD_1;
                                                      case 2:
                                                        goto switchD_0001d5ca_caseD_2;
                                                      case 3:
                                                        goto switchD_0001d5ca_caseD_3;
                                                      case 4:
                                                        goto switchD_0001d5ca_caseD_4;
                                                      case 5:
                                                        goto switchD_0001d5ca_caseD_5;
                                                      case 6:
                                                        goto switchD_0001d5ca_caseD_6;
                                                      case 7:
                                                        goto switchD_0001d5ca_caseD_7;
                                                      case 8:
                                                        goto switchD_0001d5ca_caseD_8;
                                                      case 9:
                                                        goto switchD_0001d5ca_caseD_9;
                                                      case 10:
                                                        goto switchD_0001d5ca_caseD_a;
                                                      case 0xb:
                                                        goto switchD_0001d5ca_caseD_b;
                                                      case 0xc:
                                                        goto switchD_0001d5ca_caseD_c;
                                                      case 0xd:
                                                        goto switchD_0001d5ca_caseD_d;
                                                      case 0xe:
                                                        goto switchD_0001d5ca_caseD_e;
                                                      case 0xf:
                                                        goto switchD_0001d5ca_caseD_f;
                                                      case 0x10:
                                                        goto switchD_0001d5ca_caseD_10;
                                                      case 0x11:
                                                        goto switchD_0001d5ca_caseD_11;
                                                      case 0x12:
                                                        goto switchD_0001d5ca_caseD_12;
                                                      case 0x13:
                                                        goto switchD_0001d5ca_caseD_13;
                                                      case 0x14:
                                                        goto switchD_0001d5ca_caseD_14;
                                                      case 0x15:
                                                        goto switchD_0001d5ca_caseD_15;
                                                      case 0x16:
                                                        goto switchD_0001d5ca_caseD_16;
                                                      case 0x17:
                                                        goto switchD_0001d5ca_caseD_17;
                                                      case 0x18:
                                                        goto switchD_0001d5ca_caseD_18;
                                                      case 0x19:
                                                        goto switchD_0001d5ca_caseD_19;
                                                      case 0x1a:
                                                        goto switchD_0001d5ca_caseD_1a;
                                                      case 0x1b:
                                                        goto switchD_0001d5ca_caseD_1b;
                                                      case 0x1c:
                                                        goto switchD_0001d5ca_caseD_1c;
                                                      case 0x1e:
switchD_0001d5ca_caseD_1e:
                                                        do {
                                                          uVar2 = (uint3)(uVar4 >> 8);
                                                          param_4 = param_4 + 1;
                                                          *param_3 = CONCAT31(uVar2,*(undefined1 *)
                                                                                     ((int)param_3 +
                                                                                     2)) << 0x10 |
                                                                     (uint)(uVar2 >> 8);
                                                          param_6 = param_6 + -1;
                                                          if (param_6 == 0) goto LAB_0001d5d1;
                                                          param_3 = param_3 + 0x51;
                                                          uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                          switch(uVar4 & 0xff) {
                                                          case 0:
                                                            goto switchD_0001d5ca_caseD_0;
                                                          case 1:
                                                            goto switchD_0001d5ca_caseD_1;
                                                          case 2:
                                                            goto switchD_0001d5ca_caseD_2;
                                                          case 3:
                                                            goto switchD_0001d5ca_caseD_3;
                                                          case 4:
                                                            goto switchD_0001d5ca_caseD_4;
                                                          case 5:
                                                            goto switchD_0001d5ca_caseD_5;
                                                          case 6:
                                                            goto switchD_0001d5ca_caseD_6;
                                                          case 7:
                                                            goto switchD_0001d5ca_caseD_7;
                                                          case 8:
                                                            goto switchD_0001d5ca_caseD_8;
                                                          case 9:
                                                            goto switchD_0001d5ca_caseD_9;
                                                          case 10:
                                                            goto switchD_0001d5ca_caseD_a;
                                                          case 0xb:
                                                            goto switchD_0001d5ca_caseD_b;
                                                          case 0xc:
                                                            goto switchD_0001d5ca_caseD_c;
                                                          case 0xd:
                                                            goto switchD_0001d5ca_caseD_d;
                                                          case 0xe:
                                                            goto switchD_0001d5ca_caseD_e;
                                                          case 0xf:
                                                            goto switchD_0001d5ca_caseD_f;
                                                          case 0x10:
                                                            goto switchD_0001d5ca_caseD_10;
                                                          case 0x11:
                                                            goto switchD_0001d5ca_caseD_11;
                                                          case 0x12:
                                                            goto switchD_0001d5ca_caseD_12;
                                                          case 0x13:
                                                            goto switchD_0001d5ca_caseD_13;
                                                          case 0x14:
                                                            goto switchD_0001d5ca_caseD_14;
                                                          case 0x15:
                                                            goto switchD_0001d5ca_caseD_15;
                                                          case 0x16:
                                                            goto switchD_0001d5ca_caseD_16;
                                                          case 0x17:
                                                            goto switchD_0001d5ca_caseD_17;
                                                          case 0x18:
                                                            goto switchD_0001d5ca_caseD_18;
                                                          case 0x19:
                                                            goto switchD_0001d5ca_caseD_19;
                                                          case 0x1a:
                                                            goto switchD_0001d5ca_caseD_1a;
                                                          case 0x1b:
                                                            goto switchD_0001d5ca_caseD_1b;
                                                          case 0x1c:
                                                            goto switchD_0001d5ca_caseD_1c;
                                                          case 0x1d:
                                                            goto switchD_0001d5ca_caseD_1d;
                                                          case 0x1f:
switchD_0001d5ca_caseD_1f:
                                                            do {
                                                              uVar2 = (uint3)(uVar4 >> 8);
                                                              param_4 = param_4 + 1;
                                                              *param_3 = (uint)uVar2 |
                                                                         CONCAT31(uVar2,*(undefined1
                                                                                          *)((int)
                                                  param_3 + 3)) << 0x18;
                                                  param_6 = param_6 + -1;
                                                  if (param_6 == 0) goto LAB_0001d5d1;
                                                  param_3 = param_3 + 0x51;
                                                  uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                  switch(uVar4 & 0xff) {
                                                  case 0:
                                                    goto switchD_0001d5ca_caseD_0;
                                                  case 1:
                                                    goto switchD_0001d5ca_caseD_1;
                                                  case 2:
                                                    goto switchD_0001d5ca_caseD_2;
                                                  case 3:
                                                    goto switchD_0001d5ca_caseD_3;
                                                  case 4:
                                                    goto switchD_0001d5ca_caseD_4;
                                                  case 5:
                                                    goto switchD_0001d5ca_caseD_5;
                                                  case 6:
                                                    goto switchD_0001d5ca_caseD_6;
                                                  case 7:
                                                    goto switchD_0001d5ca_caseD_7;
                                                  case 8:
                                                    goto switchD_0001d5ca_caseD_8;
                                                  case 9:
                                                    goto switchD_0001d5ca_caseD_9;
                                                  case 10:
                                                    goto switchD_0001d5ca_caseD_a;
                                                  case 0xb:
                                                    goto switchD_0001d5ca_caseD_b;
                                                  case 0xc:
                                                    goto switchD_0001d5ca_caseD_c;
                                                  case 0xd:
                                                    goto switchD_0001d5ca_caseD_d;
                                                  case 0xe:
                                                    goto switchD_0001d5ca_caseD_e;
                                                  case 0xf:
                                                    goto switchD_0001d5ca_caseD_f;
                                                  case 0x10:
                                                    goto switchD_0001d5ca_caseD_10;
                                                  case 0x11:
                                                    goto switchD_0001d5ca_caseD_11;
                                                  case 0x12:
                                                    goto switchD_0001d5ca_caseD_12;
                                                  case 0x13:
                                                    goto switchD_0001d5ca_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d5ca_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d5ca_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d5ca_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d5ca_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d5ca_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d5ca_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d5ca_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d5ca_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d5ca_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d5ca_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x20:
switchD_0001d5ca_caseD_20:
                                                    do {
                                                      param_4 = (uint *)((int)param_4 + 2);
                                                      for (uVar4 = uVar4 >> 8 & 0xff; uVar4 != 0;
                                                          uVar4 = uVar4 - 1) {
                                                        *param_3 = *param_4;
                                                        param_4 = param_4 + 1;
                                                        param_3 = param_3 + 1;
                                                      }
                                                      param_6 = param_6 + -1;
                                                      if (param_6 == 0) goto LAB_0001d5d1;
                                                      param_3 = param_3 + 0x50;
                                                      uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                      switch(uVar4 & 0xff) {
                                                      case 0:
                                                        goto switchD_0001d5ca_caseD_0;
                                                      case 1:
                                                        goto switchD_0001d5ca_caseD_1;
                                                      case 2:
                                                        goto switchD_0001d5ca_caseD_2;
                                                      case 3:
                                                        goto switchD_0001d5ca_caseD_3;
                                                      case 4:
                                                        goto switchD_0001d5ca_caseD_4;
                                                      case 5:
                                                        goto switchD_0001d5ca_caseD_5;
                                                      case 6:
                                                        goto switchD_0001d5ca_caseD_6;
                                                      case 7:
                                                        goto switchD_0001d5ca_caseD_7;
                                                      case 8:
                                                        goto switchD_0001d5ca_caseD_8;
                                                      case 9:
                                                        goto switchD_0001d5ca_caseD_9;
                                                      case 10:
                                                        goto switchD_0001d5ca_caseD_a;
                                                      case 0xb:
                                                        goto switchD_0001d5ca_caseD_b;
                                                      case 0xc:
                                                        goto switchD_0001d5ca_caseD_c;
                                                      case 0xd:
                                                        goto switchD_0001d5ca_caseD_d;
                                                      case 0xe:
                                                        goto switchD_0001d5ca_caseD_e;
                                                      case 0xf:
                                                        goto switchD_0001d5ca_caseD_f;
                                                      case 0x10:
                                                        goto switchD_0001d5ca_caseD_10;
                                                      case 0x11:
                                                        goto switchD_0001d5ca_caseD_11;
                                                      case 0x12:
                                                        goto switchD_0001d5ca_caseD_12;
                                                      case 0x13:
                                                        goto switchD_0001d5ca_caseD_13;
                                                      case 0x14:
                                                        goto switchD_0001d5ca_caseD_14;
                                                      case 0x15:
                                                        goto switchD_0001d5ca_caseD_15;
                                                      case 0x16:
                                                        goto switchD_0001d5ca_caseD_16;
                                                      case 0x17:
                                                        goto switchD_0001d5ca_caseD_17;
                                                      case 0x18:
                                                        goto switchD_0001d5ca_caseD_18;
                                                      case 0x19:
                                                        goto switchD_0001d5ca_caseD_19;
                                                      case 0x1a:
                                                        goto switchD_0001d5ca_caseD_1a;
                                                      case 0x1b:
                                                        goto switchD_0001d5ca_caseD_1b;
                                                      case 0x1c:
                                                        goto switchD_0001d5ca_caseD_1c;
                                                      case 0x1d:
                                                        goto switchD_0001d5ca_caseD_1d;
                                                      case 0x1e:
                                                        goto switchD_0001d5ca_caseD_1e;
                                                      case 0x1f:
                                                        goto switchD_0001d5ca_caseD_1f;
                                                      case 0x21:
switchD_0001d5ca_caseD_21:
                                                        do {
                                                          uVar3 = (undefined2)(uVar4 >> 0x10);
                                                          for (uVar4 = uVar4 >> 8 & 0xff; uVar4 != 0
                                                              ; uVar4 = uVar4 - 1) {
                                                            *param_3 = CONCAT22(uVar3,uVar3);
                                                            param_3 = param_3 + 1;
                                                          }
                                                          param_4 = param_4 + 1;
                                                          param_6 = param_6 + -1;
                                                          if (param_6 == 0) goto LAB_0001d5d1;
                                                          param_3 = param_3 + 0x50;
                                                          uVar4 = *param_4;
                    /* WARNING (jumptable): Sanity check requires truncation of jumptable */
                    /* WARNING: Could not find normalized switch variable to match jumptable */
                                                          switch(uVar4 & 0xff) {
                                                          case 0:
                                                            goto switchD_0001d5ca_caseD_0;
                                                          case 1:
                                                            goto switchD_0001d5ca_caseD_1;
                                                          case 2:
                                                            goto switchD_0001d5ca_caseD_2;
                                                          case 3:
                                                            goto switchD_0001d5ca_caseD_3;
                                                          case 4:
                                                            goto switchD_0001d5ca_caseD_4;
                                                          case 5:
                                                            goto switchD_0001d5ca_caseD_5;
                                                          case 6:
                                                            goto switchD_0001d5ca_caseD_6;
                                                          case 7:
                                                            goto switchD_0001d5ca_caseD_7;
                                                          case 8:
                                                            goto switchD_0001d5ca_caseD_8;
                                                          case 9:
                                                            goto switchD_0001d5ca_caseD_9;
                                                          case 10:
                                                            goto switchD_0001d5ca_caseD_a;
                                                          case 0xb:
                                                            goto switchD_0001d5ca_caseD_b;
                                                          case 0xc:
                                                            goto switchD_0001d5ca_caseD_c;
                                                          case 0xd:
                                                            goto switchD_0001d5ca_caseD_d;
                                                          case 0xe:
                                                            goto switchD_0001d5ca_caseD_e;
                                                          case 0xf:
                                                            goto switchD_0001d5ca_caseD_f;
                                                          case 0x10:
                                                            goto switchD_0001d5ca_caseD_10;
                                                          case 0x11:
                                                            goto switchD_0001d5ca_caseD_11;
                                                          case 0x12:
                                                            goto switchD_0001d5ca_caseD_12;
                                                          case 0x13:
                                                            goto switchD_0001d5ca_caseD_13;
                                                          case 0x14:
                                                            goto switchD_0001d5ca_caseD_14;
                                                          case 0x15:
                                                            goto switchD_0001d5ca_caseD_15;
                                                          case 0x16:
                                                            goto switchD_0001d5ca_caseD_16;
                                                          case 0x17:
                                                            goto switchD_0001d5ca_caseD_17;
                                                          case 0x18:
                                                            goto switchD_0001d5ca_caseD_18;
                                                          case 0x19:
                                                            goto switchD_0001d5ca_caseD_19;
                                                          case 0x1a:
                                                            goto switchD_0001d5ca_caseD_1a;
                                                          case 0x1b:
                                                            goto switchD_0001d5ca_caseD_1b;
                                                          case 0x1c:
                                                            goto switchD_0001d5ca_caseD_1c;
                                                          case 0x1d:
                                                            goto switchD_0001d5ca_caseD_1d;
                                                          case 0x1e:
                                                            goto switchD_0001d5ca_caseD_1e;
                                                          case 0x1f:
                                                            goto switchD_0001d5ca_caseD_1f;
                                                          case 0x20:
                                                            goto switchD_0001d5ca_caseD_20;
                                                          }
                                                        } while( true );
                                                      }
                                                    } while( true );
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x1d:
                                                    goto switchD_0001d5ca_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x1c:
                                                    goto switchD_0001d5ca_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d5ca_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x1b:
                                                    goto switchD_0001d5ca_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d5ca_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d5ca_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x1a:
                                                    goto switchD_0001d5ca_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d5ca_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d5ca_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d5ca_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x19:
                                                    goto switchD_0001d5ca_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d5ca_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d5ca_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d5ca_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d5ca_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x18:
                                                    goto switchD_0001d5ca_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d5ca_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d5ca_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d5ca_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d5ca_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d5ca_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x17:
                                                    goto switchD_0001d5ca_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d5ca_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d5ca_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d5ca_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d5ca_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d5ca_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d5ca_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x16:
                                                    goto switchD_0001d5ca_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d5ca_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d5ca_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d5ca_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d5ca_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d5ca_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d5ca_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d5ca_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x15:
                                                    goto switchD_0001d5ca_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d5ca_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d5ca_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d5ca_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d5ca_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d5ca_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d5ca_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d5ca_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d5ca_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x14:
                                                    goto switchD_0001d5ca_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d5ca_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d5ca_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d5ca_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d5ca_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d5ca_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d5ca_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d5ca_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d5ca_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d5ca_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x13:
                                                    goto switchD_0001d5ca_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d5ca_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d5ca_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d5ca_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d5ca_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d5ca_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d5ca_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d5ca_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d5ca_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d5ca_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d5ca_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x12:
                                                    goto switchD_0001d5ca_caseD_12;
                                                  case 0x13:
                                                    goto switchD_0001d5ca_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d5ca_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d5ca_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d5ca_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d5ca_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d5ca_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d5ca_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d5ca_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d5ca_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d5ca_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d5ca_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x11:
                                                    goto switchD_0001d5ca_caseD_11;
                                                  case 0x12:
                                                    goto switchD_0001d5ca_caseD_12;
                                                  case 0x13:
                                                    goto switchD_0001d5ca_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d5ca_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d5ca_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d5ca_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d5ca_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d5ca_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d5ca_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d5ca_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d5ca_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d5ca_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d5ca_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0x10:
                                                    goto switchD_0001d5ca_caseD_10;
                                                  case 0x11:
                                                    goto switchD_0001d5ca_caseD_11;
                                                  case 0x12:
                                                    goto switchD_0001d5ca_caseD_12;
                                                  case 0x13:
                                                    goto switchD_0001d5ca_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d5ca_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d5ca_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d5ca_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d5ca_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d5ca_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d5ca_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d5ca_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d5ca_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d5ca_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d5ca_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0xf:
                                                    goto switchD_0001d5ca_caseD_f;
                                                  case 0x10:
                                                    goto switchD_0001d5ca_caseD_10;
                                                  case 0x11:
                                                    goto switchD_0001d5ca_caseD_11;
                                                  case 0x12:
                                                    goto switchD_0001d5ca_caseD_12;
                                                  case 0x13:
                                                    goto switchD_0001d5ca_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d5ca_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d5ca_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d5ca_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d5ca_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d5ca_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d5ca_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d5ca_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d5ca_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d5ca_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d5ca_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                  case 0xe:
                                                    goto switchD_0001d5ca_caseD_e;
                                                  case 0xf:
                                                    goto switchD_0001d5ca_caseD_f;
                                                  case 0x10:
                                                    goto switchD_0001d5ca_caseD_10;
                                                  case 0x11:
                                                    goto switchD_0001d5ca_caseD_11;
                                                  case 0x12:
                                                    goto switchD_0001d5ca_caseD_12;
                                                  case 0x13:
                                                    goto switchD_0001d5ca_caseD_13;
                                                  case 0x14:
                                                    goto switchD_0001d5ca_caseD_14;
                                                  case 0x15:
                                                    goto switchD_0001d5ca_caseD_15;
                                                  case 0x16:
                                                    goto switchD_0001d5ca_caseD_16;
                                                  case 0x17:
                                                    goto switchD_0001d5ca_caseD_17;
                                                  case 0x18:
                                                    goto switchD_0001d5ca_caseD_18;
                                                  case 0x19:
                                                    goto switchD_0001d5ca_caseD_19;
                                                  case 0x1a:
                                                    goto switchD_0001d5ca_caseD_1a;
                                                  case 0x1b:
                                                    goto switchD_0001d5ca_caseD_1b;
                                                  case 0x1c:
                                                    goto switchD_0001d5ca_caseD_1c;
                                                  case 0x1d:
                                                    goto switchD_0001d5ca_caseD_1d;
                                                  case 0x1e:
                                                    goto switchD_0001d5ca_caseD_1e;
                                                  case 0x1f:
                                                    goto switchD_0001d5ca_caseD_1f;
                                                  case 0x20:
                                                    goto switchD_0001d5ca_caseD_20;
                                                  case 0x21:
                                                    goto switchD_0001d5ca_caseD_21;
                                                  }
                                                  } while( true );
                                                case 0xd:
                                                  goto switchD_0001d5ca_caseD_d;
                                                case 0xe:
                                                  goto switchD_0001d5ca_caseD_e;
                                                case 0xf:
                                                  goto switchD_0001d5ca_caseD_f;
                                                case 0x10:
                                                  goto switchD_0001d5ca_caseD_10;
                                                case 0x11:
                                                  goto switchD_0001d5ca_caseD_11;
                                                case 0x12:
                                                  goto switchD_0001d5ca_caseD_12;
                                                case 0x13:
                                                  goto switchD_0001d5ca_caseD_13;
                                                case 0x14:
                                                  goto switchD_0001d5ca_caseD_14;
                                                case 0x15:
                                                  goto switchD_0001d5ca_caseD_15;
                                                case 0x16:
                                                  goto switchD_0001d5ca_caseD_16;
                                                case 0x17:
                                                  goto switchD_0001d5ca_caseD_17;
                                                case 0x18:
                                                  goto switchD_0001d5ca_caseD_18;
                                                case 0x19:
                                                  goto switchD_0001d5ca_caseD_19;
                                                case 0x1a:
                                                  goto switchD_0001d5ca_caseD_1a;
                                                case 0x1b:
                                                  goto switchD_0001d5ca_caseD_1b;
                                                case 0x1c:
                                                  goto switchD_0001d5ca_caseD_1c;
                                                case 0x1d:
                                                  goto switchD_0001d5ca_caseD_1d;
                                                case 0x1e:
                                                  goto switchD_0001d5ca_caseD_1e;
                                                case 0x1f:
                                                  goto switchD_0001d5ca_caseD_1f;
                                                case 0x20:
                                                  goto switchD_0001d5ca_caseD_20;
                                                case 0x21:
                                                  goto switchD_0001d5ca_caseD_21;
                                                }
                                              } while( true );
                                            case 0xc:
                                              goto switchD_0001d5ca_caseD_c;
                                            case 0xd:
                                              goto switchD_0001d5ca_caseD_d;
                                            case 0xe:
                                              goto switchD_0001d5ca_caseD_e;
                                            case 0xf:
                                              goto switchD_0001d5ca_caseD_f;
                                            case 0x10:
                                              goto switchD_0001d5ca_caseD_10;
                                            case 0x11:
                                              goto switchD_0001d5ca_caseD_11;
                                            case 0x12:
                                              goto switchD_0001d5ca_caseD_12;
                                            case 0x13:
                                              goto switchD_0001d5ca_caseD_13;
                                            case 0x14:
                                              goto switchD_0001d5ca_caseD_14;
                                            case 0x15:
                                              goto switchD_0001d5ca_caseD_15;
                                            case 0x16:
                                              goto switchD_0001d5ca_caseD_16;
                                            case 0x17:
                                              goto switchD_0001d5ca_caseD_17;
                                            case 0x18:
                                              goto switchD_0001d5ca_caseD_18;
                                            case 0x19:
                                              goto switchD_0001d5ca_caseD_19;
                                            case 0x1a:
                                              goto switchD_0001d5ca_caseD_1a;
                                            case 0x1b:
                                              goto switchD_0001d5ca_caseD_1b;
                                            case 0x1c:
                                              goto switchD_0001d5ca_caseD_1c;
                                            case 0x1d:
                                              goto switchD_0001d5ca_caseD_1d;
                                            case 0x1e:
                                              goto switchD_0001d5ca_caseD_1e;
                                            case 0x1f:
                                              goto switchD_0001d5ca_caseD_1f;
                                            case 0x20:
                                              goto switchD_0001d5ca_caseD_20;
                                            case 0x21:
                                              goto switchD_0001d5ca_caseD_21;
                                            }
                                          } while( true );
                                        case 0xb:
                                          goto switchD_0001d5ca_caseD_b;
                                        case 0xc:
                                          goto switchD_0001d5ca_caseD_c;
                                        case 0xd:
                                          goto switchD_0001d5ca_caseD_d;
                                        case 0xe:
                                          goto switchD_0001d5ca_caseD_e;
                                        case 0xf:
                                          goto switchD_0001d5ca_caseD_f;
                                        case 0x10:
                                          goto switchD_0001d5ca_caseD_10;
                                        case 0x11:
                                          goto switchD_0001d5ca_caseD_11;
                                        case 0x12:
                                          goto switchD_0001d5ca_caseD_12;
                                        case 0x13:
                                          goto switchD_0001d5ca_caseD_13;
                                        case 0x14:
                                          goto switchD_0001d5ca_caseD_14;
                                        case 0x15:
                                          goto switchD_0001d5ca_caseD_15;
                                        case 0x16:
                                          goto switchD_0001d5ca_caseD_16;
                                        case 0x17:
                                          goto switchD_0001d5ca_caseD_17;
                                        case 0x18:
                                          goto switchD_0001d5ca_caseD_18;
                                        case 0x19:
                                          goto switchD_0001d5ca_caseD_19;
                                        case 0x1a:
                                          goto switchD_0001d5ca_caseD_1a;
                                        case 0x1b:
                                          goto switchD_0001d5ca_caseD_1b;
                                        case 0x1c:
                                          goto switchD_0001d5ca_caseD_1c;
                                        case 0x1d:
                                          goto switchD_0001d5ca_caseD_1d;
                                        case 0x1e:
                                          goto switchD_0001d5ca_caseD_1e;
                                        case 0x1f:
                                          goto switchD_0001d5ca_caseD_1f;
                                        case 0x20:
                                          goto switchD_0001d5ca_caseD_20;
                                        case 0x21:
                                          goto switchD_0001d5ca_caseD_21;
                                        }
                                      } while( true );
                                    case 10:
                                      goto switchD_0001d5ca_caseD_a;
                                    case 0xb:
                                      goto switchD_0001d5ca_caseD_b;
                                    case 0xc:
                                      goto switchD_0001d5ca_caseD_c;
                                    case 0xd:
                                      goto switchD_0001d5ca_caseD_d;
                                    case 0xe:
                                      goto switchD_0001d5ca_caseD_e;
                                    case 0xf:
                                      goto switchD_0001d5ca_caseD_f;
                                    case 0x10:
                                      goto switchD_0001d5ca_caseD_10;
                                    case 0x11:
                                      goto switchD_0001d5ca_caseD_11;
                                    case 0x12:
                                      goto switchD_0001d5ca_caseD_12;
                                    case 0x13:
                                      goto switchD_0001d5ca_caseD_13;
                                    case 0x14:
                                      goto switchD_0001d5ca_caseD_14;
                                    case 0x15:
                                      goto switchD_0001d5ca_caseD_15;
                                    case 0x16:
                                      goto switchD_0001d5ca_caseD_16;
                                    case 0x17:
                                      goto switchD_0001d5ca_caseD_17;
                                    case 0x18:
                                      goto switchD_0001d5ca_caseD_18;
                                    case 0x19:
                                      goto switchD_0001d5ca_caseD_19;
                                    case 0x1a:
                                      goto switchD_0001d5ca_caseD_1a;
                                    case 0x1b:
                                      goto switchD_0001d5ca_caseD_1b;
                                    case 0x1c:
                                      goto switchD_0001d5ca_caseD_1c;
                                    case 0x1d:
                                      goto switchD_0001d5ca_caseD_1d;
                                    case 0x1e:
                                      goto switchD_0001d5ca_caseD_1e;
                                    case 0x1f:
                                      goto switchD_0001d5ca_caseD_1f;
                                    case 0x20:
                                      goto switchD_0001d5ca_caseD_20;
                                    case 0x21:
                                      goto switchD_0001d5ca_caseD_21;
                                    }
                                  } while( true );
                                case 9:
                                  goto switchD_0001d5ca_caseD_9;
                                case 10:
                                  goto switchD_0001d5ca_caseD_a;
                                case 0xb:
                                  goto switchD_0001d5ca_caseD_b;
                                case 0xc:
                                  goto switchD_0001d5ca_caseD_c;
                                case 0xd:
                                  goto switchD_0001d5ca_caseD_d;
                                case 0xe:
                                  goto switchD_0001d5ca_caseD_e;
                                case 0xf:
                                  goto switchD_0001d5ca_caseD_f;
                                case 0x10:
                                  goto switchD_0001d5ca_caseD_10;
                                case 0x11:
                                  goto switchD_0001d5ca_caseD_11;
                                case 0x12:
                                  goto switchD_0001d5ca_caseD_12;
                                case 0x13:
                                  goto switchD_0001d5ca_caseD_13;
                                case 0x14:
                                  goto switchD_0001d5ca_caseD_14;
                                case 0x15:
                                  goto switchD_0001d5ca_caseD_15;
                                case 0x16:
                                  goto switchD_0001d5ca_caseD_16;
                                case 0x17:
                                  goto switchD_0001d5ca_caseD_17;
                                case 0x18:
                                  goto switchD_0001d5ca_caseD_18;
                                case 0x19:
                                  goto switchD_0001d5ca_caseD_19;
                                case 0x1a:
                                  goto switchD_0001d5ca_caseD_1a;
                                case 0x1b:
                                  goto switchD_0001d5ca_caseD_1b;
                                case 0x1c:
                                  goto switchD_0001d5ca_caseD_1c;
                                case 0x1d:
                                  goto switchD_0001d5ca_caseD_1d;
                                case 0x1e:
                                  goto switchD_0001d5ca_caseD_1e;
                                case 0x1f:
                                  goto switchD_0001d5ca_caseD_1f;
                                case 0x20:
                                  goto switchD_0001d5ca_caseD_20;
                                case 0x21:
                                  goto switchD_0001d5ca_caseD_21;
                                }
                              } while( true );
                            case 8:
                              goto switchD_0001d5ca_caseD_8;
                            case 9:
                              goto switchD_0001d5ca_caseD_9;
                            case 10:
                              goto switchD_0001d5ca_caseD_a;
                            case 0xb:
                              goto switchD_0001d5ca_caseD_b;
                            case 0xc:
                              goto switchD_0001d5ca_caseD_c;
                            case 0xd:
                              goto switchD_0001d5ca_caseD_d;
                            case 0xe:
                              goto switchD_0001d5ca_caseD_e;
                            case 0xf:
                              goto switchD_0001d5ca_caseD_f;
                            case 0x10:
                              goto switchD_0001d5ca_caseD_10;
                            case 0x11:
                              goto switchD_0001d5ca_caseD_11;
                            case 0x12:
                              goto switchD_0001d5ca_caseD_12;
                            case 0x13:
                              goto switchD_0001d5ca_caseD_13;
                            case 0x14:
                              goto switchD_0001d5ca_caseD_14;
                            case 0x15:
                              goto switchD_0001d5ca_caseD_15;
                            case 0x16:
                              goto switchD_0001d5ca_caseD_16;
                            case 0x17:
                              goto switchD_0001d5ca_caseD_17;
                            case 0x18:
                              goto switchD_0001d5ca_caseD_18;
                            case 0x19:
                              goto switchD_0001d5ca_caseD_19;
                            case 0x1a:
                              goto switchD_0001d5ca_caseD_1a;
                            case 0x1b:
                              goto switchD_0001d5ca_caseD_1b;
                            case 0x1c:
                              goto switchD_0001d5ca_caseD_1c;
                            case 0x1d:
                              goto switchD_0001d5ca_caseD_1d;
                            case 0x1e:
                              goto switchD_0001d5ca_caseD_1e;
                            case 0x1f:
                              goto switchD_0001d5ca_caseD_1f;
                            case 0x20:
                              goto switchD_0001d5ca_caseD_20;
                            case 0x21:
                              goto switchD_0001d5ca_caseD_21;
                            }
                          } while( true );
                        case 7:
                          goto switchD_0001d5ca_caseD_7;
                        case 8:
                          goto switchD_0001d5ca_caseD_8;
                        case 9:
                          goto switchD_0001d5ca_caseD_9;
                        case 10:
                          goto switchD_0001d5ca_caseD_a;
                        case 0xb:
                          goto switchD_0001d5ca_caseD_b;
                        case 0xc:
                          goto switchD_0001d5ca_caseD_c;
                        case 0xd:
                          goto switchD_0001d5ca_caseD_d;
                        case 0xe:
                          goto switchD_0001d5ca_caseD_e;
                        case 0xf:
                          goto switchD_0001d5ca_caseD_f;
                        case 0x10:
                          goto switchD_0001d5ca_caseD_10;
                        case 0x11:
                          goto switchD_0001d5ca_caseD_11;
                        case 0x12:
                          goto switchD_0001d5ca_caseD_12;
                        case 0x13:
                          goto switchD_0001d5ca_caseD_13;
                        case 0x14:
                          goto switchD_0001d5ca_caseD_14;
                        case 0x15:
                          goto switchD_0001d5ca_caseD_15;
                        case 0x16:
                          goto switchD_0001d5ca_caseD_16;
                        case 0x17:
                          goto switchD_0001d5ca_caseD_17;
                        case 0x18:
                          goto switchD_0001d5ca_caseD_18;
                        case 0x19:
                          goto switchD_0001d5ca_caseD_19;
                        case 0x1a:
                          goto switchD_0001d5ca_caseD_1a;
                        case 0x1b:
                          goto switchD_0001d5ca_caseD_1b;
                        case 0x1c:
                          goto switchD_0001d5ca_caseD_1c;
                        case 0x1d:
                          goto switchD_0001d5ca_caseD_1d;
                        case 0x1e:
                          goto switchD_0001d5ca_caseD_1e;
                        case 0x1f:
                          goto switchD_0001d5ca_caseD_1f;
                        case 0x20:
                          goto switchD_0001d5ca_caseD_20;
                        case 0x21:
                          goto switchD_0001d5ca_caseD_21;
                        }
                      } while( true );
                    case 6:
                      goto switchD_0001d5ca_caseD_6;
                    case 7:
                      goto switchD_0001d5ca_caseD_7;
                    case 8:
                      goto switchD_0001d5ca_caseD_8;
                    case 9:
                      goto switchD_0001d5ca_caseD_9;
                    case 10:
                      goto switchD_0001d5ca_caseD_a;
                    case 0xb:
                      goto switchD_0001d5ca_caseD_b;
                    case 0xc:
                      goto switchD_0001d5ca_caseD_c;
                    case 0xd:
                      goto switchD_0001d5ca_caseD_d;
                    case 0xe:
                      goto switchD_0001d5ca_caseD_e;
                    case 0xf:
                      goto switchD_0001d5ca_caseD_f;
                    case 0x10:
                      goto switchD_0001d5ca_caseD_10;
                    case 0x11:
                      goto switchD_0001d5ca_caseD_11;
                    case 0x12:
                      goto switchD_0001d5ca_caseD_12;
                    case 0x13:
                      goto switchD_0001d5ca_caseD_13;
                    case 0x14:
                      goto switchD_0001d5ca_caseD_14;
                    case 0x15:
                      goto switchD_0001d5ca_caseD_15;
                    case 0x16:
                      goto switchD_0001d5ca_caseD_16;
                    case 0x17:
                      goto switchD_0001d5ca_caseD_17;
                    case 0x18:
                      goto switchD_0001d5ca_caseD_18;
                    case 0x19:
                      goto switchD_0001d5ca_caseD_19;
                    case 0x1a:
                      goto switchD_0001d5ca_caseD_1a;
                    case 0x1b:
                      goto switchD_0001d5ca_caseD_1b;
                    case 0x1c:
                      goto switchD_0001d5ca_caseD_1c;
                    case 0x1d:
                      goto switchD_0001d5ca_caseD_1d;
                    case 0x1e:
                      goto switchD_0001d5ca_caseD_1e;
                    case 0x1f:
                      goto switchD_0001d5ca_caseD_1f;
                    case 0x20:
                      goto switchD_0001d5ca_caseD_20;
                    case 0x21:
                      goto switchD_0001d5ca_caseD_21;
                    }
                  } while( true );
                case 5:
                  goto switchD_0001d5ca_caseD_5;
                case 6:
                  goto switchD_0001d5ca_caseD_6;
                case 7:
                  goto switchD_0001d5ca_caseD_7;
                case 8:
                  goto switchD_0001d5ca_caseD_8;
                case 9:
                  goto switchD_0001d5ca_caseD_9;
                case 10:
                  goto switchD_0001d5ca_caseD_a;
                case 0xb:
                  goto switchD_0001d5ca_caseD_b;
                case 0xc:
                  goto switchD_0001d5ca_caseD_c;
                case 0xd:
                  goto switchD_0001d5ca_caseD_d;
                case 0xe:
                  goto switchD_0001d5ca_caseD_e;
                case 0xf:
                  goto switchD_0001d5ca_caseD_f;
                case 0x10:
                  goto switchD_0001d5ca_caseD_10;
                case 0x11:
                  goto switchD_0001d5ca_caseD_11;
                case 0x12:
                  goto switchD_0001d5ca_caseD_12;
                case 0x13:
                  goto switchD_0001d5ca_caseD_13;
                case 0x14:
                  goto switchD_0001d5ca_caseD_14;
                case 0x15:
                  goto switchD_0001d5ca_caseD_15;
                case 0x16:
                  goto switchD_0001d5ca_caseD_16;
                case 0x17:
                  goto switchD_0001d5ca_caseD_17;
                case 0x18:
                  goto switchD_0001d5ca_caseD_18;
                case 0x19:
                  goto switchD_0001d5ca_caseD_19;
                case 0x1a:
                  goto switchD_0001d5ca_caseD_1a;
                case 0x1b:
                  goto switchD_0001d5ca_caseD_1b;
                case 0x1c:
                  goto switchD_0001d5ca_caseD_1c;
                case 0x1d:
                  goto switchD_0001d5ca_caseD_1d;
                case 0x1e:
                  goto switchD_0001d5ca_caseD_1e;
                case 0x1f:
                  goto switchD_0001d5ca_caseD_1f;
                case 0x20:
                  goto switchD_0001d5ca_caseD_20;
                case 0x21:
                  goto switchD_0001d5ca_caseD_21;
                }
              } while( true );
            case 4:
              goto switchD_0001d5ca_caseD_4;
            case 5:
              goto switchD_0001d5ca_caseD_5;
            case 6:
              goto switchD_0001d5ca_caseD_6;
            case 7:
              goto switchD_0001d5ca_caseD_7;
            case 8:
              goto switchD_0001d5ca_caseD_8;
            case 9:
              goto switchD_0001d5ca_caseD_9;
            case 10:
              goto switchD_0001d5ca_caseD_a;
            case 0xb:
              goto switchD_0001d5ca_caseD_b;
            case 0xc:
              goto switchD_0001d5ca_caseD_c;
            case 0xd:
              goto switchD_0001d5ca_caseD_d;
            case 0xe:
              goto switchD_0001d5ca_caseD_e;
            case 0xf:
              goto switchD_0001d5ca_caseD_f;
            case 0x10:
              goto switchD_0001d5ca_caseD_10;
            case 0x11:
              goto switchD_0001d5ca_caseD_11;
            case 0x12:
              goto switchD_0001d5ca_caseD_12;
            case 0x13:
              goto switchD_0001d5ca_caseD_13;
            case 0x14:
              goto switchD_0001d5ca_caseD_14;
            case 0x15:
              goto switchD_0001d5ca_caseD_15;
            case 0x16:
              goto switchD_0001d5ca_caseD_16;
            case 0x17:
              goto switchD_0001d5ca_caseD_17;
            case 0x18:
              goto switchD_0001d5ca_caseD_18;
            case 0x19:
              goto switchD_0001d5ca_caseD_19;
            case 0x1a:
              goto switchD_0001d5ca_caseD_1a;
            case 0x1b:
              goto switchD_0001d5ca_caseD_1b;
            case 0x1c:
              goto switchD_0001d5ca_caseD_1c;
            case 0x1d:
              goto switchD_0001d5ca_caseD_1d;
            case 0x1e:
              goto switchD_0001d5ca_caseD_1e;
            case 0x1f:
              goto switchD_0001d5ca_caseD_1f;
            case 0x20:
              goto switchD_0001d5ca_caseD_20;
            case 0x21:
              goto switchD_0001d5ca_caseD_21;
            }
          } while( true );
        case 3:
          goto switchD_0001d5ca_caseD_3;
        case 4:
          goto switchD_0001d5ca_caseD_4;
        case 5:
          goto switchD_0001d5ca_caseD_5;
        case 6:
          goto switchD_0001d5ca_caseD_6;
        case 7:
          goto switchD_0001d5ca_caseD_7;
        case 8:
          goto switchD_0001d5ca_caseD_8;
        case 9:
          goto switchD_0001d5ca_caseD_9;
        case 10:
          goto switchD_0001d5ca_caseD_a;
        case 0xb:
          goto switchD_0001d5ca_caseD_b;
        case 0xc:
          goto switchD_0001d5ca_caseD_c;
        case 0xd:
          goto switchD_0001d5ca_caseD_d;
        case 0xe:
          goto switchD_0001d5ca_caseD_e;
        case 0xf:
          goto switchD_0001d5ca_caseD_f;
        case 0x10:
          goto switchD_0001d5ca_caseD_10;
        case 0x11:
          goto switchD_0001d5ca_caseD_11;
        case 0x12:
          goto switchD_0001d5ca_caseD_12;
        case 0x13:
          goto switchD_0001d5ca_caseD_13;
        case 0x14:
          goto switchD_0001d5ca_caseD_14;
        case 0x15:
          goto switchD_0001d5ca_caseD_15;
        case 0x16:
          goto switchD_0001d5ca_caseD_16;
        case 0x17:
          goto switchD_0001d5ca_caseD_17;
        case 0x18:
          goto switchD_0001d5ca_caseD_18;
        case 0x19:
          goto switchD_0001d5ca_caseD_19;
        case 0x1a:
          goto switchD_0001d5ca_caseD_1a;
        case 0x1b:
          goto switchD_0001d5ca_caseD_1b;
        case 0x1c:
          goto switchD_0001d5ca_caseD_1c;
        case 0x1d:
          goto switchD_0001d5ca_caseD_1d;
        case 0x1e:
          goto switchD_0001d5ca_caseD_1e;
        case 0x1f:
          goto switchD_0001d5ca_caseD_1f;
        case 0x20:
          goto switchD_0001d5ca_caseD_20;
        case 0x21:
          goto switchD_0001d5ca_caseD_21;
        }
      } while( true );
    case 2:
      goto switchD_0001d5ca_caseD_2;
    case 3:
      goto switchD_0001d5ca_caseD_3;
    case 4:
      goto switchD_0001d5ca_caseD_4;
    case 5:
      goto switchD_0001d5ca_caseD_5;
    case 6:
      goto switchD_0001d5ca_caseD_6;
    case 7:
      goto switchD_0001d5ca_caseD_7;
    case 8:
      goto switchD_0001d5ca_caseD_8;
    case 9:
      goto switchD_0001d5ca_caseD_9;
    case 10:
      goto switchD_0001d5ca_caseD_a;
    case 0xb:
      goto switchD_0001d5ca_caseD_b;
    case 0xc:
      goto switchD_0001d5ca_caseD_c;
    case 0xd:
      goto switchD_0001d5ca_caseD_d;
    case 0xe:
      goto switchD_0001d5ca_caseD_e;
    case 0xf:
      goto switchD_0001d5ca_caseD_f;
    case 0x10:
      goto switchD_0001d5ca_caseD_10;
    case 0x11:
      goto switchD_0001d5ca_caseD_11;
    case 0x12:
      goto switchD_0001d5ca_caseD_12;
    case 0x13:
      goto switchD_0001d5ca_caseD_13;
    case 0x14:
      goto switchD_0001d5ca_caseD_14;
    case 0x15:
      goto switchD_0001d5ca_caseD_15;
    case 0x16:
      goto switchD_0001d5ca_caseD_16;
    case 0x17:
      goto switchD_0001d5ca_caseD_17;
    case 0x18:
      goto switchD_0001d5ca_caseD_18;
    case 0x19:
      goto switchD_0001d5ca_caseD_19;
    case 0x1a:
      goto switchD_0001d5ca_caseD_1a;
    case 0x1b:
      goto switchD_0001d5ca_caseD_1b;
    case 0x1c:
      goto switchD_0001d5ca_caseD_1c;
    case 0x1d:
      goto switchD_0001d5ca_caseD_1d;
    case 0x1e:
      goto switchD_0001d5ca_caseD_1e;
    case 0x1f:
      goto switchD_0001d5ca_caseD_1f;
    case 0x20:
      goto switchD_0001d5ca_caseD_20;
    case 0x21:
      goto switchD_0001d5ca_caseD_21;
    }
  } while( true );
}

