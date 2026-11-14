// ModPlayer.java - locate the MOD/module code: find xrefs to the 'M.K.' and
// '6CHN' constants (file 0x98204/0x9822c -> VA +0xF000), decompile all
// referencing functions.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Reference;

public class ModPlayer extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[] datas = { 0x98204L + 0xF000L, 0x9822cL + 0xF000L };
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        java.util.Set<Function> done = new java.util.HashSet<Function>();
        for (long a : datas) {
            println("DATAREF @" + Long.toHexString(a));
            Reference[] refs = getReferencesTo(toAddr(a));
            println("  refs: " + refs.length);
            for (Reference r : refs) {
                Function f = getFunctionContaining(r.getFromAddress());
                println("  ref from " + r.getFromAddress() + " in " +
                        (f == null ? "?" : f.getName() + " " + f.getBody()));
                if (f == null || !done.add(f)) continue;
                DecompileResults res = decomp.decompileFunction(f, 240, monitor);
                if (res.decompileCompleted()) {
                    println("/* ---- " + f.getName() + " ---- */");
                    println(res.getDecompiledFunction().getC());
                } else println("failed: " + res.getErrorMessage());
            }
        }
        decomp.dispose();
    }
}
