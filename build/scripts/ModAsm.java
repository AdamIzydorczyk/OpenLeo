// ModAsm.java - disassemble the code referencing the 'M.K.' constant:
// linear disassembly of 0x12a80..0x13300 (module loader region).
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.listing.Instruction;

public class ModAsm extends GhidraScript {
    @Override
    public void run() throws Exception {
        AddressSet set = new AddressSet(toAddr(0x12a80L), toAddr(0x13300L));
        for (Instruction ins : currentProgram.getListing()
                 .getInstructions(set, true))
            println("  " + ins.getAddress() + "  " + ins);
    }
}
