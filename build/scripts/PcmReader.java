// PcmReader.java - decompile the stream open/seek/decode helpers used by the
// PCM ctor: FUN_00044a50 (bind header), FUN_00044ab0, FUN_00044b80 (row
// decode), FUN_000104b8 (read field), FUN_00076a64 (alloc).
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;

public class PcmReader extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[] targets = { 0x44a50L, 0x44ab0L, 0x44b80L, 0x44c00L, 0x104b8L, 0x76a64L, 0x57990L, 0x57ae0L };
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        java.util.Set<Function> done = new java.util.HashSet<Function>();
        for (long t : targets) {
            Function f = getFunctionContaining(toAddr(t));
            println("TARGET " + Long.toHexString(t) + " -> " +
                    (f == null ? "NO FUNCTION" : f.getEntryPoint() + " .. " + f.getBody()));
            if (f == null || !done.add(f)) continue;
            DecompileResults res = decomp.decompileFunction(f, 180, monitor);
            if (res.decompileCompleted()) {
                println("/* ---- " + f.getName() + " ---- */");
                println(res.getDecompiledFunction().getC());
            } else println("failed: " + res.getErrorMessage());
        }
        decomp.dispose();
    }
}
