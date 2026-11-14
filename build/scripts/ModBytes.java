// ModBytes.java - find code that touches MOD-layout constants (1084, 1268,
// 950, 1080, 30, 29) so we can read the true file-parsing routine.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;

public class ModBytes extends GhidraScript {
    @Override
    public void run() throws Exception {
        long[] consts = { 1084, 1268, 950, 1080, 256, 3300 };
        java.util.Set<Long> fns = new java.util.LinkedHashSet<>();
        ghidra.program.model.mem.MemoryBlock tb =
            currentProgram.getMemory().getBlock(".text");
        if (tb == null)
            for (ghidra.program.model.mem.MemoryBlock b :
                 currentProgram.getMemory().getBlocks())
                if (b.isExecute() && b.getSize() > 0x10000) { tb = b; break; }
        ghidra.program.model.address.AddressSet code =
            new ghidra.program.model.address.AddressSet(
                tb.getStart(), tb.getEnd());
        for (long c : consts) {
            for (Instruction ins : currentProgram.getListing()
                     .getInstructions(code, true)) {
                boolean hit = false;
                for (Object o : ins.getOpObjects(1))
                    if (o instanceof ghidra.program.model.scalar.Scalar
                        && ((ghidra.program.model.scalar.Scalar) o)
                            .getValue().longValue() == c) hit = true;
                if (ins.toString().contains("0x" + Long.toHexString(c)))
                    hit = true;
                if (hit) {
                    Function f = getFunctionContaining(ins.getAddress());
                    if (f != null && f.getBody().getMinAddress()
                            .getOffset() >= 0x12000
                        && f.getBody().getMaxAddress().getOffset() <= 0x16000)
                        fns.add(f.getEntryPoint().getOffset());
                }
            }
        }
        println("candidate fns: " + fns);
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        for (long a : fns) {
            Function f = getFunctionAt(toAddr(a));
            println("FN " + f.getName() + " " + f.getBody());
            DecompileResults res = decomp.decompileFunction(f, 300, monitor);
            if (res.decompileCompleted())
                println(res.getDecompiledFunction().getC());
            else println("failed: " + res.getErrorMessage());
        }
        decomp.dispose();
    }
}
