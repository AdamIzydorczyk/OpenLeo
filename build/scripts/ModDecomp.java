// ModDecomp.java - create functions for the unidentified MOD loader region
// and decompile the whole player: header parse (0x12eb0), pattern load
// (0x12d14), sample load (0x132b0), mixer/tick (FUN_00013600, 0x12ce0,
// 0x12c40, 0x152b0, 0x15484).
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;

public class ModDecomp extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[] starts = { 0x12eb0L, 0x13600L, 0x152b0L, 0x15484L, 0x15240L,
                          0x12d14L, 0x131b8L, 0x13b50L, 0x13bd4L };
        for (long s : starts) {
            if (getFunctionAt(toAddr(s)) == null &&
                getFunctionContaining(toAddr(s)) == null) {
                try {
                    createFunction(toAddr(s), "MODFN_" + Long.toHexString(s));
                    println("created MODFN_" + Long.toHexString(s));
                } catch (Exception e) { println("create fail " + Long.toHexString(s) + ": " + e); }
            }
        }
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        java.util.Set<Function> done = new java.util.HashSet<Function>();
        for (long s : starts) {
            Function f = getFunctionContaining(toAddr(s));
            if (f == null || !done.add(f)) continue;
            DecompileResults res = decomp.decompileFunction(f, 300, monitor);
            if (res.decompileCompleted()) {
                println("/* ---- " + f.getName() + " " + f.getBody() + " ---- */");
                println(res.getDecompiledFunction().getC());
            } else println("failed: " + res.getErrorMessage());
        }
        decomp.dispose();
    }
}
