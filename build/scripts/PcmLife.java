// PcmLife.java - decompile the PCM object lifecycle: loader caller
// (0x573c4), the parallax neighbors 0x5741e/0x574d7/0x574e7/0x574f3 and
// the vtable-dtor region, to see field usage and where blit + table fit.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;

public class PcmLife extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[] targets = { 0x573c4L, 0x5741eL, 0x574d7L, 0x574e7L, 0x574f3L };
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
