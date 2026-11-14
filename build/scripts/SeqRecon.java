// SeqRecon.java - decode path for .SEQ / id=1 container payloads.
// Decompiles the llrs reader gate FUN_000592e0, the boot opener
// FUN_0005c880, plus every caller of the byte-reader FUN_00058b40 and
// FUN_000592e0 (those host the payload decompressor candidates).
// Run: analyzeHeadless build/ghidra LEOPROJ -process leo.elf -noanalysis \
//        -scriptPath build/scripts -postScript SeqRecon.java
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.symbol.Reference;

public class SeqRecon extends GhidraScript {
    static final String[] TARGETS = {
        "FUN_000592e0", "FUN_0005c880", "FUN_0005c670", "FUN_0005c680",
        "FUN_0005c690", "FUN_0005c6a0", "FUN_00058b40",
    };

    @Override
    public void run() throws Exception {
        println("=== SeqRecon start");
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        java.util.Set<Function> done = new java.util.HashSet<Function>();
        int budget = 40;

        for (String name : TARGETS) {
            Function f = findFunction(name);
            if (f == null) { println("MISSING " + name); continue; }
            if (done.add(f)) { dump(decomp, f); budget--; }
            // callers of each target
            Reference[] refs = getReferencesTo(f.getEntryPoint());
            int cr = 0;
            for (Reference r : refs) {
                if (monitor.isCancelled()) break;
                Function cf = getFunctionContaining(r.getFromAddress());
                if (cf == null || cf.equals(f)) continue;
                println("CALLER " + r.getFromAddress() + " -> " + name
                        + " in " + cf.getName() + " @ " + cf.getEntryPoint());
                if (done.add(cf) && budget-- > 0) dump(decomp, cf);
                if (++cr > 60) break;
            }
        }
        println("=== SeqRecon done, " + done.size() + " functions decompiled");
        decomp.dispose();
    }

    Function findFunction(String name) {
        FunctionIterator it = currentProgram.getFunctionManager()
            .getFunctions(true);
        while (it.hasNext()) {
            Function f = it.next();
            if (f.getName().equals(name)) return f;
        }
        return null;
    }

    void dump(DecompInterface decomp, Function f) {
        DecompileResults res = decomp.decompileFunction(f, 120, monitor);
        if (res.decompileCompleted()) {
            println("/* ---- decompile " + f.getName() + " @ "
                    + f.getEntryPoint() + " ---- */");
            println(res.getDecompiledFunction().getC());
        } else {
            println("decompile failed " + f.getEntryPoint() + ": "
                    + res.getErrorMessage());
        }
    }
}
