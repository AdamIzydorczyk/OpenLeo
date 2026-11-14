// SeqElem.java - the per-element .SEQ parser FUN_00041ef0 (+ its helpers
// FUN_00043d40 header init, FUN_00021910 list init, FUN_00041e00 region).
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.symbol.Reference;

public class SeqElem extends GhidraScript {
    @Override
    public void run() throws Exception {
        println("=== SeqElem start");
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        java.util.Set<Function> done = new java.util.HashSet<Function>();
        String[] targets = { "FUN_00041ef0", "FUN_00043d40", "FUN_00021910" };
        for (String name : targets) {
            Function f = findFunction(name);
            if (f == null) { println("MISSING " + name); continue; }
            if (done.add(f)) dump(decomp, f);
            for (Reference r : getReferencesTo(f.getEntryPoint())) {
                Function cf = getFunctionContaining(r.getFromAddress());
                if (cf != null && !cf.equals(f) && done.add(cf)) {
                    println("CALLER " + r.getFromAddress() + " -> " + name
                            + " in " + cf.getName() + " @ "
                            + cf.getEntryPoint());
                    dump(decomp, cf);
                }
            }
        }
        println("=== SeqElem done, " + done.size() + " functions");
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
