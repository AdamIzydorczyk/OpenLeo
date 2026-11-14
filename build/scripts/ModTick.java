// ModTick.java - who drives the MOD mixer (FUN_00013600) and at what rate:
// disassemble around callers + decompile the mixer tick and the play loop.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.symbol.Reference;

public class ModTick extends GhidraScript {
    @Override
    public void run() throws Exception {
        for (String s : new String[] { "0x13600", "0x12d14" }) {
            Address t = toAddr(Long.decode(s));
            println("== xrefs to " + t);
            for (Reference r : currentProgram.getReferenceManager().getReferencesTo(t))
                println("   from " + r.getFromAddress() + " " + r.getReferenceType());
        }
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        for (long a : new long[] { 0x13600L, 0x13450L, 0x12f00L }) {
            Function f = getFunctionContaining(toAddr(a));
            if (f == null) { println("no fn @ " + Long.toHexString(a)); continue; }
            println("FN " + f.getName() + " " + f.getBody());
            DecompileResults res = decomp.decompileFunction(f, 300, monitor);
            if (res.decompileCompleted())
                println(res.getDecompiledFunction().getC());
            else println("failed: " + res.getErrorMessage());
        }
        // context around each caller of FUN_00013600
        for (Reference r : currentProgram.getReferenceManager()
                 .getReferencesTo(toAddr(0x13600L))) {
            AddressSet set = new AddressSet(r.getFromAddress().add(-24),
                                            r.getFromAddress().add(8));
            println("---- around caller " + r.getFromAddress());
            for (Instruction ins : currentProgram.getListing()
                     .getInstructions(set, true))
                println("  " + ins.getAddress() + "  " + ins);
        }
        decomp.dispose();
    }
}
