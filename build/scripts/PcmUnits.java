// PcmUnits.java - pin the PCM load units: header field reader (FUN_00044a50),
// RLE chunk reader (FUN_00044b80), buffer allocator (thunk_FUN_0005c320 and
// its target), and one blit caller site if identifiable.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Reference;

public class PcmUnits extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[] targets = { 0x44a50L, 0x44ab0L, 0x44b80L, 0x5c320L, 0x579e0L };
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
        // who calls the blitter FUN_00056c72?
        Function blit = getFunctionAt(toAddr(0x56c72L));
        if (blit != null) {
            Reference[] refs = getReferencesTo(blit.getEntryPoint());
            for (Reference r : refs)
                println("BLITTER CALLER ref @ " + r.getFromAddress() + " in " +
                        getFunctionContaining(r.getFromAddress()));
        }
        decomp.dispose();
    }
}
