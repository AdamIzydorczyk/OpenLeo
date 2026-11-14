// DesRefs.java - find who references the .DES key string pool (0x92d00..0x92f20)
// and its pointer table (0xaaa00..0xaaa90); decompile hosting functions.
// Run: analyzeHeadless build/ghidra LEOPROJ -process leo.elf -noanalysis \
//        -scriptPath build/scripts -postScript DesRefs.java
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;

public class DesRefs extends GhidraScript {
    @Override
    public void run() throws Exception {
        long pool = 0x92d00L, poolEnd = 0x92f20L;
        long tbl = 0xaaa00L,  tblEnd = 0xaaa90L;
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        java.util.Set<Function> done = new java.util.HashSet<Function>();

        Memory mem = currentProgram.getMemory();
        for (MemoryBlock blk : mem.getBlocks()) {
            byte[] data = new byte[(int) blk.getSize()];
            mem.getBytes(blk.getStart(), data);
            for (int i = 0; i + 3 < data.length; i += 4) {
                long v = (data[i] & 0xffL) | ((data[i+1] & 0xffL) << 8)
                       | ((data[i+2] & 0xffL) << 16) | ((data[i+3] & 0xffL) << 24);
                if ((v >= pool && v < poolEnd) || (v >= tbl && v < tblEnd)) {
                    Function f = getFunctionContaining(blk.getStart().add(i));
                    if (f == null)
                        continue;
                    if (done.add(f)) {
                        println("HOST " + f.getEntryPoint() + "  (dword@"
                                + blk.getStart().add(i) + " -> " + Long.toHexString(v) + ")");
                        DecompileResults res = decomp.decompileFunction(f, 120, monitor);
                        if (res.decompileCompleted()) {
                            println("/* ---- " + f.getEntryPoint() + " ---- */");
                            println(res.getDecompiledFunction().getC());
                        } else {
                            println("decompile failed " + f.getEntryPoint());
                        }
                    }
                }
            }
            if (monitor.isCancelled()) break;
        }
        println("desrefs: " + done.size() + " functions");
        decomp.dispose();
    }
}
