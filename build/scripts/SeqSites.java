// SeqSites.java - raw-disassemble the functions CONTAINING the given call
// sites (the .SEQ loader users = animation/scene engine entry points).
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;

public class SeqSites extends GhidraScript {
    @Override
    public void run() throws Exception {
        println("=== SeqSites start");
        String[] sites = { "000440ae", "0005ec56", "0005fcf8", "00060a6c" };
        java.util.Set<Function> seen = new java.util.LinkedHashSet<>();
        for (String s : sites) {
            Address a = currentProgram.getAddressFactory().getAddress(s);
            Function f = currentProgram.getFunctionManager()
                .getFunctionContaining(a);
            println("site " + s + " -> " +
                    (f == null ? "NO FUNCTION" :
                     f.getName() + " @ " + f.getEntryPoint()));
            if (f != null) seen.add(f);
        }
        for (Function f : seen) {
            println("/* ---- asm " + f.getName() + " @ " + f.getEntryPoint()
                    + " ---- */");
            InstructionIterator bi = currentProgram.getListing()
                .getInstructions(f.getBody(), true);
            while (bi.hasNext() && !monitor.isCancelled()) {
                Instruction ins = bi.next();
                println("  " + ins.getAddress() + "  " + ins);
            }
        }
        println("=== SeqSites done");
    }
}
