// SeqParse.java - find the .SEQ / text-script parser via its string tokens,
// then decompile the hosting functions.
// String ranges (Ghidra vaddrs = leo.img offset + 0x10000):
//   0x92135..0x922d9  seq token names (BY_X..ANTI_POCISK) + scanf formats
//   0x93190..0x93274  numbers.seq + animation names (A_JUMP..A_ATAK)
//   0x9168a..0x91696  ".seq" extension + "id = %d"
//   0x926a0..0x926c0  "Cannot open status file" (STATUS.LEO loaders ref)
// Run: analyzeHeadless build/ghidra LEOPROJ -process leo.elf -noanalysis \
//        -scriptPath build/scripts -postScript SeqParse.java
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.scalar.Scalar;
import ghidra.program.model.symbol.Reference;

public class SeqParse extends GhidraScript {
    static final long[][] RANGES = {
        {0x92135L, 0x922daL},
        {0x93180L, 0x93279L},
        {0x91680L, 0x916a0L},
    };

    @Override
    public void run() throws Exception {
        println("=== SeqParse start");
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        java.util.Set<Function> done = new java.util.HashSet<Function>();
        int budget = 45, scanned = 0;

        for (Instruction ins :
                currentProgram.getListing().getInstructions(true)) {
            if (monitor.isCancelled() || budget <= 0) break;
            Long hit = null;
            for (int opi = 0; opi < ins.getNumOperands(); opi++)
                for (Object o : ins.getOpObjects(opi))
                    if (o instanceof Scalar) {
                        long v = ((Scalar) o).getUnsignedValue();
                        for (long[] r : RANGES)
                            if (v >= r[0] && v < r[1]) hit = v;
                    }
            // also lea/offset loads: constant operand already covered by Scalar
            if (hit != null) {
                Function f = getFunctionContaining(ins.getAddress());
                println("REF " + ins.getAddress() + " -> 0x"
                        + Long.toHexString(hit) + "  " + ins
                        + (f != null ? "  in " + f.getName() : ""));
                if (f != null && done.add(f) && budget-- > 0)
                    dump(decomp, f);
            }
            // string literal data refs (reference manager side)
            if (++scanned % 20000 == 0) println("... scanned " + scanned);
        }
        // references whose 'to' address lands in ranges (lea-style data refs)
        for (long[] r : RANGES) {
            Address start = currentProgram.getAddressFactory()
                .getDefaultAddressSpace().getAddress(r[0]);
            Address end = currentProgram.getAddressFactory()
                .getDefaultAddressSpace().getAddress(r[1] - 1);
            Address a = start;
            int n = 0;
            while (a != null && a.compareTo(end) <= 0 && n++ < 400
                    && !monitor.isCancelled() && budget > 0) {
                for (Reference ref : getReferencesTo(a)) {
                    Function f = getFunctionContaining(ref.getFromAddress());
                    if (f != null && done.add(f)) {
                        println("DATREF " + a + " used by " + f.getName()
                                + " @ " + f.getEntryPoint());
                        if (budget-- > 0) dump(decomp, f);
                    }
                }
                a = a.next();
            }
        }
        println("=== SeqParse done, " + done.size() + " functions");
        decomp.dispose();
    }

    void dump(DecompInterface decomp, Function f) {
        DecompileResults res = decomp.decompileFunction(f, 120, monitor);
        if (res.decompileCompleted()) {
            println("/* ---- decompile " + f.getName() + " @ "
                    + f.getEntryPoint() + " ---- */");
            println(res.getDecompiledFunction().getC());
        } else {
            println("decompile failed " + f.getEntryPoint() + ": "
                    + res.getErrorMessage());
        }
    }
}
