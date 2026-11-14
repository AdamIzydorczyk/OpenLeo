// CallersOf.java - print all xref sources (code) to given hex addresses.
// Run: analyzeHeadless build/ghidra LEOPROJ -process leo.elf -noanalysis \
//        -scriptPath build/scripts -postScript CallersOf.java 0x4c8a0 0x4dde0
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.symbol.Reference;

public class CallersOf extends GhidraScript {
    @Override
    public void run() throws Exception {
        for (String a : getScriptArgs()) {
            Address t = toAddr(Long.decode(a));
            println("== xrefs to " + t);
            for (Reference r : getReferenceManager().getReferencesTo(t))
                println("   from " + r.getFromAddress() + " type=" + r.getReferenceType());
        }
    }
}
