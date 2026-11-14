// FuncDump.java - decompile functions given as script args (hex addresses).
// Run: analyzeHeadless build/ghidra LEOPROJ -process leo.elf -noanalysis \
//        -scriptPath build/scripts -postScript "FuncDump.java 0x1fef0 0x25510"
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;

public class FuncDump extends GhidraScript {
    @Override
    public void run() throws Exception {
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        for (String a : getScriptArgs()) {
            long t = Long.decode(a);
            Function f = getFunctionContaining(toAddr(t));
            if (f == null) { println("NOFUNC " + a); continue; }
            DecompileResults res = decomp.decompileFunction(f, 180, monitor);
            println("/* ==== " + f.getEntryPoint() + " size=" + f.getBody().getNumAddresses() + " ==== */");
            if (res.decompileCompleted())
                println(res.getDecompiledFunction().getC());
            else
                println("failed: " + res.getErrorMessage());
        }
        decomp.dispose();
    }
}
