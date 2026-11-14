// ModTables.java - dump module-table constants from data:
// 0xa7272 (61 x u16 period->note table), 0xa73a8 (vibrato/sin 32 x u8),
// 0xa731c (16 x ? fine/vol table), signature table at 0xa7204 (10 x 10B).
import ghidra.app.script.GhidraScript;

public class ModTables extends GhidraScript {
    @Override
    public void run() throws Exception {
        print("PERIOD61:");
        for (int i = 0; i < 61; i++)
            print(" " + (getShort(toAddr(0xa7272L + 2L * i)) & 0xFFFF));
        println("");
        print("SIGTAB:");
        for (int i = 0; i < 10; i++) {
            StringBuilder sb = new StringBuilder(" ");
            for (int j = 0; j < 10; j++) {
                byte b = getByte(toAddr(0xa7204L + 10L * i + j));
                if (b >= 32 && b < 127) sb.append((char) b);
                else sb.append('.');
            }
            print(sb.toString());
        }
        println("");
        print("TAB31C:");
        for (int i = 0; i < 16; i++)
            print(" " + (getShort(toAddr(0xa731cL + 2L * i)) & 0xFFFF));
        println("");
        print("VIB32:");
        for (int i = 0; i < 32; i++)
            print(" " + (getByte(toAddr(0xa73a8L + i)) & 0xFF));
        println("");
        print("A72A4-jmptab:");
        for (int i = 0; i < 30; i++)
            print(" " + Integer.toHexString(getInt(toAddr(0xa72a4L + 4L * i)) & 0xFFFFFF));
        println("");
    }
}
