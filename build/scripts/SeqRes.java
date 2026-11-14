// SeqRes.java - raw disassembly of the vector reserve/setter helpers used by
// the .SEQ element readers (FUN_000214b0 / FUN_00021500 / FUN_000215a0).
// Purpose: pin whether the +2 byte slack per (palette+sprite) block belongs
// to a stream read inside the reserve functions or to the reads themselves.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;

public class SeqRes extends GhidraScript {
    @Override
    public void run() throws Exception {
        println("=== SeqRes start");
        String[] targets = { "FUN_000214b0", "FUN_00021500", "FUN_000215a0",
                             "FUN_00022a70" };
        for (String name : targets) {
            ghidra.program.model.listing.Function f = null;
            java.util.Iterator<ghidra.program.model.listing.Function> it =
                currentProgram.getFunctionManager().getFunctions(true);
            while (it.hasNext()) {
                ghidra.program.model.listing.Function cand = it.next();
                if (cand.getName().equals(name)) { f = cand; break; }
            }
            if (f == null) { println("MISSING " + name); continue; }
            println("/* ---- asm " + name + " @ " + f.getEntryPoint()
                    + " ---- */");
            InstructionIterator ii =
                currentProgram.getListing().getInstructions(f.getBody(), true);
            while (ii.hasNext() && !monitor.isCancelled()) {
                Instruction ins = ii.next();
                println("  " + ins.getAddress() + "  " + ins);
            }
        }
        println("=== SeqRes done");
    }
}
