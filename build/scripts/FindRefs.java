// FindRefs.java - Ghidra headless script: find xrefs to an address (pointer dwords
// in data + immediate operands in code), then decompile the hosting functions.
// Run: analyzeHeadless build/ghidra LEOPROJ -process leo.elf -noanalysis \
//        -scriptPath build/scripts -postScript FindRefs.java
// Target address is hardcoded below (headless cannot pass script args).
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.symbol.Reference;

public class FindRefs extends GhidraScript {
    @Override
    public void run() throws Exception {
        long target = 0x92980L;   // bottom of the anti-tamper/XMS string pool
        long rangeEnd = 0x92c00L; // any pointer/imm landing inside the pool counts
        long tbl = 0xa9a00L;      // pointer table that references the pool
        long tblEnd = 0xa9a40L;   // ...and who references the table itself
        String[] args = getScriptArgs();
        if (args.length > 0) target = Long.decode(args[0]);
        Address tgt = toAddr(target);

        println("=== functions total: " + currentProgram.getFunctionManager().getFunctionCount());

        // 1) explicit references known to Ghidra (data pointers it already typed)
        for (Reference r : currentProgram.getReferenceManager().getReferencesTo(tgt)) {
            println("XREF(ref) " + r.getFromAddress() + " type=" + r.getReferenceType());
        }

        // 2) raw scan: little-endian dwords inside pool range, anywhere in memory
        Memory mem = currentProgram.getMemory();
        int hits = 0;
        for (ghidra.program.model.mem.MemoryBlock blk : mem.getBlocks()) {
            byte[] data = new byte[(int)blk.getSize()];
            mem.getBytes(blk.getStart(), data);
            for (int i = 0; i + 3 < data.length; i += 4) {
                long v = (data[i] & 0xffL) | ((data[i+1] & 0xffL) << 8)
                       | ((data[i+2] & 0xffL) << 16) | ((data[i+3] & 0xffL) << 24);
                if ((v >= target && v < rangeEnd) || (v >= tbl && v < tblEnd)) {
                    Address at = blk.getStart().add(i);
                    Instruction ins = currentProgram.getListing().getInstructionAt(at);
                    println("XREF(dword) " + at + (ins != null ? " in-code: " + ins : " data")
                            + " -> " + Long.toHexString(v));
                    if (++hits > 60) break;
                }
            }
            if (monitor.isCancelled() || hits > 60) break;
        }

        // 3) decompile every function containing a hit
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        java.util.Set<Function> done = new java.util.HashSet<Function>();
        for (Reference r : currentProgram.getReferenceManager().getReferencesTo(tgt)) {
            Function f = getFunctionContaining(r.getFromAddress());
            if (f != null && done.add(f)) dump(decomp, f);
        }
        // also try: any instruction with an absolute/displacement operand landing
        // in or near the pool, or based on its 4K page (Watcom often uses [base+disp])
        long pageBase = target & ~0xfffL;
        long pageEnd = pageBase + 0x1000L;
        InstructionIterator itts = currentProgram.getListing().getInstructions(true);
        while (itts.hasNext() && !monitor.isCancelled()) {
            Instruction ins = itts.next();
            for (int opi = 0; opi < ins.getNumOperands(); opi++) {
                boolean match = false;
                for (Object o : ins.getOpObjects(opi)) {
                    if (o instanceof ghidra.program.model.scalar.Scalar) {
                        long u = ((ghidra.program.model.scalar.Scalar)o).getUnsignedValue();
                        if ((u >= target && u < rangeEnd) || (u >= tbl && u < tblEnd)
                            || (u >= pageBase && u < pageEnd && u > 0x1000)) match = true;
                    }
                }
                if (match) {
                    Function f = getFunctionContaining(ins.getAddress());
                    println("XREF(imm) " + ins.getAddress() + "  " + ins + (f != null ? "  in " + f.getEntryPoint() : ""));
                    if (f != null && done.add(f)) dump(decomp, f);
                }
            }
        }
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
