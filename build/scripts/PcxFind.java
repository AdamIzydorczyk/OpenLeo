// PcxFind.java - xref + decompile users of the ".pcx"/"name = %s" strings
// (0x90f80..0x90fa0) and the ".seq"/"id = %d" strings (0x91680..0x916a0),
// i.e. the asset loader front-ends.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Reference;

public class PcxFind extends GhidraScript {
    static final long[][] RANGES = { {0x90f70L, 0x90fa8L}, {0x91670L, 0x916b0L} };

    @Override
    public void run() throws Exception {
        println("=== PcxFind start");
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        java.util.Set<Function> done = new java.util.HashSet<Function>();
        for (long[] r : RANGES) {
            Address a = toAddr(r[0]), end = toAddr(r[1] - 1);
            while (a.compareTo(end) <= 0 && !monitor.isCancelled()) {
                for (Reference ref : getReferencesTo(a)) {
                    Function f = getFunctionContaining(ref.getFromAddress());
                    if (f != null && done.add(f)) {
                        println("XREF " + ref.getFromAddress() + " -> " + a
                                + " in " + f.getName() + " @ " + f.getEntryPoint());
                        dump(decomp, f);
                    }
                }
                a = a.next();
            }
        }
        println("=== PcxFind done, " + done.size() + " functions");
        decomp.dispose();
    }

    void dump(DecompInterface decomp, Function f) {
        DecompileResults res = decomp.decompileFunction(f, 120, monitor);
        if (res.decompileCompleted()) {
            println("/* ---- decompile " + f.getName() + " @ " + f.getEntryPoint() + " ---- */");
            println(res.getDecompiledFunction().getC());
        } else {
            println("decompile failed " + f.getEntryPoint() + ": " + res.getErrorMessage());
        }
    }
}
