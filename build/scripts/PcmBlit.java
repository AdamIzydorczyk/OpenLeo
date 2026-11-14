// PcmBlit.java - decompile the PCM ctor/blitter/parallax functions.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;

public class PcmBlit extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[] targets = { 0x44b80L, 0x579e0L, 0x44ab0L };
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
