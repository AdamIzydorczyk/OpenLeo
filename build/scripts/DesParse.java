// DesParse.java - decompile the function(s) containing the .DES key parsing
// xrefs at 0x4ca25..0x4cf95 and everything they call that touches file I/O.
// Run: analyzeHeadless build/ghidra LEOPROJ -process leo.elf -noanalysis \
//        -scriptPath build/scripts -postScript DesParse.java
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;

public class DesParse extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[] sites = { 0x4ca25L, 0x4cd1eL, 0x4ce53L };
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        java.util.Set<String> done = new java.util.HashSet<String>();
        for (long s : sites) {
            Function f = getFunctionContaining(toAddr(s));
            if (f == null) { println("no function at " + Long.toHexString(s)); continue; }
            println("SITE " + Long.toHexString(s) + " -> " + f.getEntryPoint()
                    + " body=" + f.getBody().getNumAddresses());
            dump(decomp, f, done, 0);
        }
        decomp.dispose();
    }

    void dump(DecompInterface decomp, Function f, java.util.Set<String> done, int depth) {
        if (depth > 1 || !done.add(f.getEntryPoint().toString()))
            return;
        if (f.getBody().getNumAddresses() < 40)
            return;                                  // skip tiny thunks/CRT
        DecompileResults res = decomp.decompileFunction(f, 180, monitor);
        println("/* ==== " + f.getEntryPoint() + " size=" + f.getBody().getNumAddresses() + " ==== */");
        if (res.decompileCompleted())
            println(res.getDecompiledFunction().getC());
        else
            println("failed: " + res.getErrorMessage());
        if (depth < 1)
            for (Function g : f.getCalledFunctions(monitor))
                dump(decomp, g, done, depth + 1);
    }
}
