// ModLoad.java - full decompile of the MOD loader chain (open/inst-dir/
// sample-load/pos-loop) to pin sample-offset + length + rate semantics.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;

public class ModLoad extends GhidraScript {
    @Override
    public void run() throws Exception {
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        for (long a : new long[] { 0x12eb0L, 0x132b0L, 0x13300L, 0x131b8L,
                                   0x1338cL, 0x13440L, 0x1317cL, 0x13510L }) {
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
