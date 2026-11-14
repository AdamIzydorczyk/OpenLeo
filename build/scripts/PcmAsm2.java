// PcmAsm2.java - disassemble the PCM blitter FUN_00056c72, the parallax
// updater FUN_00057460, the 80-byte row copier FUN_0001d528 and the header
// reader FUN_00044a50 so the exact field/arg plumbing is visible.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;

public class PcmAsm2 extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[] targets = { 0x56c72L, 0x57460L, 0x1d528L, 0x44a50L, 0x44b80L };
        for (long t : targets) {
            Function f = getFunctionContaining(toAddr(t));
            if (f == null) { println("MISSING " + Long.toHexString(t)); continue; }
            println("/* ---- asm " + f.getName() + " @ " + f.getEntryPoint()
                    + " ---- */");
            InstructionIterator ii =
                currentProgram.getListing().getInstructions(f.getBody(), true);
            while (ii.hasNext() && !monitor.isCancelled()) {
                Instruction ins = ii.next();
                println("  " + ins.getAddress() + "  " + ins);
            }
        }
    }
}
