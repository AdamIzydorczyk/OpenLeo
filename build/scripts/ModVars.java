// ModVars.java - find functions writing MOD-state globals (0xaee99 patcnt,
// 0xaee94 nch, 0xaee9d, 0xaefce dirbase) then decompile the header parser.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Reference;

public class ModVars extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[] globals = { 0xaee99L, 0xaee94L, 0xaee9dL, 0xaefceL, 0xaee8cL };
        java.util.Set<Long> fns = new java.util.LinkedHashSet<>();
        for (long g : globals) {
            println("== refs to " + Long.toHexString(g));
            for (Reference r : currentProgram.getReferenceManager()
                     .getReferencesTo(toAddr(g))) {
                println("   " + r.getFromAddress() + " " + r.getReferenceType());
                Function f = getFunctionContaining(r.getFromAddress());
                if (f != null) fns.add(f.getEntryPoint().getOffset());
            }
        }
        println("fns: " + fns);
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        for (long a : fns) {
            if (a < 0x12000 || a > 0x14000) continue;
            Function f = getFunctionAt(toAddr(a));
            println("FN " + f.getName() + " " + f.getBody());
            DecompileResults res = decomp.decompileFunction(f, 300, monitor);
            if (res.decompileCompleted())
                println(res.getDecompiledFunction().getC());
            else println("failed: " + res.getErrorMessage());
        }
        decomp.dispose();
    }
}
