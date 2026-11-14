// SeqFn.java - raw-disassemble functions by exact entry address.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;

public class SeqFn extends GhidraScript {
    @Override
    public void run() throws Exception {
        println("=== SeqFn start");
        String[] targets = { "000426a0", "000424d0", "000420b0", "00042140" };
        for (String t : targets) {
            Address a = currentProgram.getAddressFactory().getAddress(t);
            println("/* ---- asm @ " + t + " ---- */");
            InstructionIterator bi =
                currentProgram.getListing().getInstructions(a, true);
            int c = 0;
            while (bi.hasNext() && !monitor.isCancelled() && c < 400) {
                Instruction ins = bi.next();
                println("  " + ins.getAddress() + "  " + ins);
                c++;
                String s = ins.toString();
                if ((s.equals("RET") || s.startsWith("RET")) && c > 8) {
                    Address nx = ins.getAddress().add(1);
                    Instruction ni = currentProgram.getListing()
                        .getInstructionAt(nx);
                    if (ni != null && !ni.toString().startsWith("PUSH")
                        && !ni.toString().startsWith("MOV EAX,ESP")
                        && !ni.toString().startsWith("SUB ESP")
                        && !ni.toString().startsWith("XOR")
                        && !ni.toString().startsWith("LEA")
                        && !ni.toString().startsWith("JMP"))
                        break;
                }
            }
        }
        println("=== SeqFn done");
    }
}
