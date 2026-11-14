// PcmAsm3.java - exact disassembly of the PCM copy primitive FUN_0001d528
// and the blitter FUN_00056c72 to settle pointer arithmetic units.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;

public class PcmAsm3 extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[] fs = { 0x1d528L, 0x56c72L };
        for (long t : fs) {
            Function f = getFunctionContaining(toAddr(t));
            println("ASM of " + f.getName() + " " + f.getBody());
            for (Instruction ins : currentProgram.getListing()
                     .getInstructions(f.getBody(), true))
                println("  " + ins.getAddress() + "  " + ins);
        }
    }
}
