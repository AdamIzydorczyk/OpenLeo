// MnuRefs.java - find code referencing the menu/name string tables, then decompile
// hosting functions. Ranges (Ghidra flat vaddrs = leo.img offset + 0x10000):
//   0x925b0..0x92fa0  data-filename string pool (.is/.mnu/.mod/level names)
//   0xa97c0..0xa98c0  ANIMACJE/status.leo/mapka.mod strings (.data)
//   0xa99c0..0x99ae0  pointer table of cutscene+menu names (per-language triplets)
// Run: analyzeHeadless build/ghidra LEOPROJ -process leo.elf -noanalysis \
//        -scriptPath build/scripts -postScript MnuRefs.java
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.scalar.Scalar;

public class MnuRefs extends GhidraScript {
    static final long[][] RANGES = {
        {0x925b0L, 0x92fa0L},
        {0xa97c0L, 0xa98c0L},
        {0xa99c0L, 0x99ae0L},
    };
    static boolean inRange(long v) {
        for (long[] r : RANGES) if (v >= r[0] && v < r[1]) return true;
        return false;
    }

    @Override
    public void run() throws Exception {
        println("=== MnuRefs start");
        // 1) data dwords pointing into the string pools (pointer tables)
        Memory mem = currentProgram.getMemory();
        int hits = 0;
        for (ghidra.program.model.mem.MemoryBlock blk : mem.getBlocks()) {
            byte[] data = new byte[(int)blk.getSize()];
            mem.getBytes(blk.getStart(), data);
            for (int i = 0; i + 3 < data.length; i += 4) {
                long v = (data[i] & 0xffL) | ((data[i+1] & 0xffL) << 8)
                       | ((data[i+2] & 0xffL) << 16) | ((data[i+3] & 0xffL) << 24);
                if (v >= 0x925b0L && v < 0x92fa0L) {
                    Address at = blk.getStart().add(i);
                    println("PTR(dword) " + at + " -> " + Long.toHexString(v));
                    if (++hits > 400) break;
                }
            }
            if (monitor.isCancelled() || hits > 400) break;
        }
        // 2) instructions with scalar operands landing in any range
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        java.util.Set<Function> done = new java.util.HashSet<Function>();
        int n = 0;
        InstructionIterator its = currentProgram.getListing().getInstructions(true);
        while (its.hasNext() && !monitor.isCancelled()) {
            Instruction ins = its.next();
            boolean match = false;
            for (int opi = 0; opi < ins.getNumOperands(); opi++)
                for (Object o : ins.getOpObjects(opi))
                    if (o instanceof Scalar && inRange(((Scalar)o).getUnsignedValue()))
                        match = true;
            if (match) {
                Function f = getFunctionContaining(ins.getAddress());
                println("REF(ins) " + ins.getAddress() + "  " + ins
                        + (f != null ? "  in " + f.getEntryPoint() : ""));
                if (f != null && done.add(f)) dump(decomp, f);
                if (++n > 300) break;
            }
        }
        println("=== MnuRefs done, " + done.size() + " functions decompiled");
        decomp.dispose();
    }

    void dump(DecompInterface decomp, Function f) {
        DecompileResults res = decomp.decompileFunction(f, 120, monitor);
        if (res.decompileCompleted()) {
            println("/* ---- decompile " + f.getName() + " @ " + f.getEntryPoint() + " ---- */");
            println(res.getDecompiledFunction().getC());
        } else {
            println("decompile failed " + f.getEntryPoint() + ": " + res.getErrorMessage());
        }
    }
}
