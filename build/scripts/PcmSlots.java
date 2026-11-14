// PcmSlots.java - decompile the PCM layer vtable (0xa2790) slot handlers:
// 0x578d0 [1], 0x56c70 [2 blit thunk], 0x56ad0 [3 load-window?],
// 0x57960 [4], 0x57970 [6], plus the a277c-table extras 0x56d40/0x571c0/
// 0x570b0/0x56ef0/0x57910 to pin the row-streaming + table usage.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;

public class PcmSlots extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[] targets = { 0x56ad0L, 0x56ef0L, 0x56d40L, 0x571c0L, 0x570b0L,
                           0x57910L, 0x578d0L, 0x57960L };
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
