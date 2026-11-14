// SeqAsm.java - raw disassembly of the .SEQ element reader chain. The
// decompiler lost the cdecl/regparm argument plumbing; the instruction
// listing shows exactly how many bytes each stream read consumes and
// where decoded fields are stored.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;

public class SeqAsm extends GhidraScript {
    @Override
    public void run() throws Exception {
        println("=== SeqAsm start");
        String[] targets = { "FUN_00041e90", "FUN_00041ef0", "FUN_00041f60",
                             "FUN_00041d40", "FUN_00041da0", "FUN_000420b0",
                             "FUN_00041e00", "FUN_00043e70", "FUN_00043cf0",
                             "FUN_00076ab0" };
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
        println("=== SeqAsm done");
    }
}
