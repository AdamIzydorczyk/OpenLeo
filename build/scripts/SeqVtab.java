// SeqVtab.java - dump the vtable at 0xa0c08 (installed at obj+0x33 by the
// .SEQ element ctor FUN_00041ef0) and raw-disassemble each target function:
// these are the per-element update/render methods = the interpreter core.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;

public class SeqVtab extends GhidraScript {
    @Override
    public void run() throws Exception {
        println("=== SeqVtab start");
        long base = 0xa0c08L;
        long[] slots = new long[10];
        for (int i = 0; i < slots.length; i++) {
            int v = currentProgram.getMemory()
                .getInt(currentProgram.getAddressFactory()
                           .getAddress(Long.toHexString(base + 4L * i)));
            slots[i] = v & 0xffffffffL;
            println(String.format("slot %d -> %08x", i, slots[i]));
        }
        java.util.Set<Long> seen = new java.util.LinkedHashSet<>();
        for (long s : slots) {
            if (s == 0 || s >= 0x100000 || !seen.add(s)) continue;
            Address a = currentProgram.getAddressFactory()
                .getAddress(Long.toHexString(s));
            Function f = currentProgram.getFunctionManager()
                .getFunctionContaining(a);
            Address start = f != null ? f.getEntryPoint() : a;
            int n = f != null ? -1 : 40;
            println("/* ---- asm vtab+" + " @ " + start + " (" +
                    (f == null ? "no-function, raw 40 insn" :
                     f.getName()) + ") ---- */");
            ghidra.program.model.listing.InstructionIterator bi =
                currentProgram.getListing().getInstructions(start, true);
            int c = 0;
            while (bi.hasNext() && !monitor.isCancelled() &&
                   (n < 0 || c < n)) {
                ghidra.program.model.listing.Instruction ins = bi.next();
                if (f != null && f.getBody().contains(ins.getAddress()) == false
                    && c > 0) break;
                println("  " + ins.getAddress() + "  " + ins);
                c++;
            }
        }
        println("=== SeqVtab done");
    }
}
