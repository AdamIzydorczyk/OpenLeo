// ModAsm2.java - disassemble 0x132b0..0x13d00 plus xrefs to module state
// globals 0xaee94 (channels), 0xaee99 (pattern count), 0xaee90 (file obj).
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Reference;

public class ModAsm2 extends GhidraScript {
    @Override
    public void run() throws Exception {
        AddressSet set = new AddressSet(toAddr(0x13300L), toAddr(0x13d00L));
        for (Instruction ins : currentProgram.getListing()
                 .getInstructions(set, true))
            println("  " + ins.getAddress() + "  " + ins);
        long[] gs = { 0xaee94L, 0xaee99L, 0xaee9dL };
        for (long g : gs) {
            for (Reference r : getReferencesTo(toAddr(g))) {
                Function f = getFunctionContaining(r.getFromAddress());
                println("XREF " + Long.toHexString(g) + " from " +
                        r.getFromAddress() + " in " + (f == null ? "?" : f.getName()));
            }
        }
    }
}
