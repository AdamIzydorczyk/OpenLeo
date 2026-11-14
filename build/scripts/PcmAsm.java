// PcmAsm.java - disassemble FUN_00056b20 (PCM ctor) to see exact file I/O
// sequence + immediates, and decompile FUN_00044a50 / FUN_00044ab0 (the
// read helpers it calls).
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;

public class PcmAsm extends GhidraScript {
    @Override
    public void run() throws Exception {
        Function f = getFunctionContaining(toAddr(0x56b20L));
        println("ASM of " + f.getName() + " " + f.getBody());
        ghidra.program.model.listing.InstructionIterator ii =
            currentProgram.getListing().getInstructions(f.getBody(), true);
        while (ii.hasNext() && !monitor.isCancelled()) {
            Instruction ins = ii.next();
            println("  " + ins.getAddress() + "  " + ins);
        }
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        long[] targets = { 0x44a50L, 0x44ab0L, 0x49af0L, 0x49ba0L };
        java.util.Set<Function> done = new java.util.HashSet<Function>();
        for (long t : targets) {
            Function g = getFunctionContaining(toAddr(t));
            println("TARGET " + Long.toHexString(t) + " -> " +
                    (g == null ? "NO FUNCTION" : g.getEntryPoint()));
            if (g == null || !done.add(g)) continue;
            DecompileResults res = decomp.decompileFunction(g, 180, monitor);
            if (res.decompileCompleted()) {
                println("/* ---- " + g.getName() + " ---- */");
                println(res.getDecompiledFunction().getC());
            } else println("failed: " + res.getErrorMessage());
        }
        decomp.dispose();
    }
}
