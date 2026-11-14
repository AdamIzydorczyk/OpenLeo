// ModTick2.java - decompile the functions that CONTAIN the two mixer-call
// sites (0x13741, 0x13146) so we can see the refill size + pacing.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;

public class ModTick2 extends GhidraScript {
    @Override
    public void run() throws Exception {
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        for (long a : new long[] { 0x13741L, 0x13146L, 0x136a0L }) {
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
