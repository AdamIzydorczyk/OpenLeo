// ModFns.java - disassemble the regions that reference MAPKA.MOD (0x5df04)
// and deatd.mod (0x4ffdd) plus decompile containing functions.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;

public class ModFns extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[][] ranges = { { 0x5dd80L, 0x5df60L }, { 0x4fe80L, 0x50040L } };
        for (long[] r : ranges) {
            AddressSet set = new AddressSet(toAddr(r[0]), toAddr(r[1]));
            for (Instruction ins : currentProgram.getListing()
                     .getInstructions(set, true))
                println("  " + ins.getAddress() + "  " + ins);
        }
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        for (long a : new long[] { 0x5df04L, 0x4ffddL }) {
            Function f = getFunctionContaining(toAddr(a));
            if (f == null) { println("no fn @ " + Long.toHexString(a)); continue; }
            println("FN " + f.getName() + " " + f.getBody());
            DecompileResults res = decomp.decompileFunction(f, 300, monitor);
            if (res.decompileCompleted())
                println(res.getDecompiledFunction().getC());
            else println("failed: " + res.getErrorMessage());
        }
        decomp.dispose();
    }
}
