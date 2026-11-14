// SeqImm.java - find every instruction whose text mentions 43e70 (any
// immediate form) and every memory location holding the little-endian
// dword 0x00043e70 (thunks / vtable slots).
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.mem.MemoryBlock;

public class SeqImm extends GhidraScript {
    @Override
    public void run() throws Exception {
        println("=== SeqImm start");
        InstructionIterator ii =
            currentProgram.getListing().getInstructions(true);
        while (ii.hasNext() && !monitor.isCancelled()) {
            Instruction ins = ii.next();
            String s = ins.toString();
            if (s.contains("43e70"))
                println("imm  " + ins.getAddress() + "  " + s);
        }
        byte[] pat = { 0x70, 0x3e, 0x04, 0x00 };
        for (MemoryBlock b : currentProgram.getMemory().getBlocks()) {
            long len = b.getSize();
            if (len > 64L * 1024 * 1024) continue;
            byte[] buf = new byte[(int) len];
            try {
                currentProgram.getMemory().getBytes(b.getStart(), buf);
            } catch (Exception e) { continue; }
            for (int i = 0; i + 4 <= buf.length; i++) {
                boolean hit = true;
                for (int k = 0; k < 4; k++)
                    if (buf[i + k] != pat[k]) { hit = false; break; }
                if (hit)
                    println("ptr  " + b.getStart().add(i)
                            + "  (block " + b.getName() + ")");
            }
        }
        println("=== SeqImm done");
    }
}
