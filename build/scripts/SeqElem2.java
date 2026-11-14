// SeqElem2.java - .SEQ element decode core: FUN_00041f60 (init from stream?)
// and FUN_00043cf0 (element reader) + their callees one level deep.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.RefType;

public class SeqElem2 extends GhidraScript {
    @Override
    public void run() throws Exception {
        println("=== SeqElem2 start");
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        java.util.Set<Function> done = new java.util.HashSet<Function>();
        String[] targets = { "FUN_00041f60", "FUN_00043cf0" };
        for (String name : targets) {
            Function f = findFunction(name);
            if (f == null) { println("MISSING " + name); continue; }
            if (done.add(f)) dump(decomp, f);
            // callees (one level)
            for (Reference r : currentProgram.getReferenceManager()
                    .getReferencesFrom(f.getEntryPoint())) {
                if (r.getReferenceType().isCall()) {
                    Function cf = getFunctionContaining(r.getToAddress());
                    if (cf != null && done.add(cf)) dump(decomp, cf);
                }
            }
        }
        println("=== SeqElem2 done, " + done.size() + " functions");
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
