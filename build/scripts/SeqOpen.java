// SeqOpen.java - decompile the ".seq" asset open/log path:
// FUN_00041320 (ref'd PUSH 0x91689 = "id = %d" string area) and FUN_00043e70
// (list builder called under the .seq opener), plus every caller of
// FUN_00044020 (the ".seq" list loader) and users of 0x91695 (".seq" ext).
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.symbol.Reference;

public class SeqOpen extends GhidraScript {
    @Override
    public void run() throws Exception {
        println("=== SeqOpen start");
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        java.util.Set<Function> done = new java.util.HashSet<Function>();
        String[] targets = { "FUN_00041320", "FUN_00043e70", "FUN_00044020",
                             "FUN_00044120" };
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
        // xrefs to the ".seq" extension string itself
        Address a = toAddr(0x91695L);
        for (Reference r : getReferencesTo(a)) {
            Function f = getFunctionContaining(r.getFromAddress());
            if (f != null && done.add(f)) {
                println("SEQSTR used by " + f.getName() + " @ "
                        + f.getEntryPoint());
                dump(decomp, f);
            }
        }
        println("=== SeqOpen done, " + done.size() + " functions");
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
