// SeqLeaf.java - leaf primitives of the .SEQ element decode:
// FUN_00041d40 (flag byte read?), FUN_00041da0 / FUN_000420b0 (conditional
// variant parsers), FUN_00041e00 (per-element payload), and the stream-read
// helpers they call (one level down).
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.symbol.Reference;

public class SeqLeaf extends GhidraScript {
    @Override
    public void run() throws Exception {
        println("=== SeqLeaf start");
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        java.util.Set<Function> done = new java.util.HashSet<Function>();
        int budget = 30;
        java.util.List<Function> queue = new java.util.ArrayList<Function>();
        String[] targets = { "FUN_00041d40", "FUN_00041da0", "FUN_000420b0",
                             "FUN_00041e00" };
        for (String name : targets) {
            Function f = findFunction(name);
            if (f == null) { println("MISSING " + name); continue; }
            if (done.add(f)) { queue.add(f); }
        }
        // level-1 callees of FUN_00041f60 too
        Function top = findFunction("FUN_00041f60");
        if (top != null)
            for (Reference r :
                    currentProgram.getReferenceManager()
                        .getReferencesFrom(top.getEntryPoint())) {
                if (r.getReferenceType().isCall()) {
                    Function cf = getFunctionContaining(r.getToAddress());
                    if (cf != null && done.add(cf)) queue.add(cf);
                }
            }
        int qi = 0;
        while (qi < queue.size() && budget > 0 && !monitor.isCancelled()) {
            Function f = queue.get(qi++);
            dump(decomp, f);
            budget--;
            for (Reference r :
                    currentProgram.getReferenceManager()
                        .getReferencesFrom(f.getEntryPoint())) {
                if (r.getReferenceType().isCall()) {
                    Function cf = getFunctionContaining(r.getToAddress());
                    if (cf != null && done.add(cf) && queue.size() < 40)
                        queue.add(cf);
                }
            }
        }
        println("=== SeqLeaf done, " + done.size() + " functions");
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
