// SeqHdr.java - disassembly of the .SEQ file/header layer: open + header
// read + vector helpers, to pin exactly which file bytes feed the element
// stream (FUN_00041f60) and what the 3 u16 header words mean.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;

public class SeqHdr extends GhidraScript {
    @Override
    public void run() throws Exception {
        println("=== SeqHdr start");
        String[] targets = { "FUN_00043d40", "FUN_00044020", "FUN_00044120",
                             "FUN_00057990", "FUN_00057a00", "FUN_00021910",
                             "FUN_00049aa0", "FUN_00022910", "FUN_0005c320",
                             "FUN_0005c6b0", "FUN_0007b791" };
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
        println("=== SeqHdr done");
    }
}
