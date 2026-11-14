// PcmVt.java - dump vtable contents around the PCM class and the strip
// manager classes, and show every call site in FUN_000574d0/0x574e0 region
// plus which functions the vtables at 0x9d574 / 0xa277c / 0xa2790 / 0xa27a4
// point at.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;

public class PcmVt extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[] vts = { 0x9d574L, 0xa277cL, 0xa2790L, 0xa27a4L };
        for (long v : vts) {
            println("== vtable @ " + Long.toHexString(v));
            for (int i = 0; i < 8; i++) {
                ghidra.program.model.listing.Data d =
                    currentProgram.getListing().getDefinedDataAt(
                        toAddr(v + i * 4));
                Object ov = (d == null) ? null : d.getValue();
                println("   [" + i + "] -> " + ov);
            }
        }
        long[] fns = { 0x574d0L, 0x574e0L, 0x57570L, 0x57c60L };
        for (long t : fns) {
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
