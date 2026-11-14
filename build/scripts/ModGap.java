// ModGap.java - create+decompile the function(s) in the unanalyzed gap
// 0x12e2f..0x1317b (MOD header parser: title/instrument dir/dedup).
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;

public class ModGap extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[] starts = { 0x12e2fL, 0x12eb0L, 0x13000L };
        for (long a : starts) {
            try {
                if (getFunctionContaining(toAddr(a)) == null)
                    createFunction(toAddr(a), "MODGAP_" + Long.toHexString(a));
            } catch (Exception e) {
                println("create failed @ " + Long.toHexString(a) + ": " + e);
            }
        }
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        for (long a : starts) {
            Function f = getFunctionContaining(toAddr(a));
            if (f == null) { println("still no fn @ " + Long.toHexString(a)); continue; }
            println("FN " + f.getName() + " " + f.getBody());
            DecompileResults res = decomp.decompileFunction(f, 600, monitor);
            if (res.decompileCompleted())
                println(res.getDecompiledFunction().getC());
            else println("failed: " + res.getErrorMessage());
        }
        decomp.dispose();
    }
}
