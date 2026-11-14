// PcmDraw.java - xrefs + disasm around the PCM blit slot wrapper 0x56c70
// and the level draw loop that calls it.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.symbol.Reference;

public class PcmDraw extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[] ts = { 0x56c70L, 0x9d570L, 0xa2790L, 0xa277cL };
        for (long t : ts) {
            Address a = toAddr(t);
            println("== xrefs to " + a);
            for (Reference r : currentProgram.getReferenceManager().getReferencesTo(a))
                println("   from " + r.getFromAddress() + " " + r.getReferenceType());
        }
        for (long t : ts) {
            Function f = getFunctionContaining(toAddr(t));
            if (f == null) { println("NOFN " + Long.toHexString(t)); continue; }
            println("/* ---- asm " + f.getName() + " " + f.getEntryPoint()
                    + " " + f.getBody() + " ---- */");
            java.util.Iterator<Instruction> ii =
                currentProgram.getListing().getInstructions(f.getBody(), true);
            while (ii.hasNext() && !monitor.isCancelled()) {
                Instruction ins = ii.next();
                println("  " + ins.getAddress() + "  " + ins);
            }
        }
    }
}
