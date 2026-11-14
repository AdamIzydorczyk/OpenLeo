// SeqReader.java - decompile the llrs payload stream path to find whether
// id=1 (.SEQ) payloads are decompressed on read:
//   FUN_00058a00 (called by FUN_00058b40 payload-open), FUN_00058980,
//   FUN_00058c50 (slot lookup), FUN_00059ba0 (container hdr parse),
//   FUN_00057d30 / FUN_00076d20 / FUN_00076a69 (text token reader used
//   by the BY_X parser FUN_00051913), FUN_00043e70 (.seq list builder).
// Run: analyzeHeadless build/ghidra LEOPROJ -process leo.elf -noanalysis \
//        -scriptPath build/scripts -postScript SeqReader.java
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;

public class SeqReader extends GhidraScript {
    static final String[] TARGETS = {
        "FUN_00058a00", "FUN_00058980", "FUN_00058c50", "FUN_00059ba0",
        "FUN_00057d30", "FUN_00076d20", "FUN_00076a69", "FUN_00043e70",
    };

    @Override
    public void run() throws Exception {
        println("=== SeqReader start");
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        for (String name : TARGETS) {
            Function f = findFunction(name);
            if (f == null) { println("MISSING " + name); continue; }
            dump(decomp, f);
        }
        println("=== SeqReader done");
        decomp.dispose();
    }

    Function findFunction(String name) {
        FunctionIterator it = currentProgram.getFunctionManager()
            .getFunctions(true);
        while (it.hasNext()) {
            Function f = it.next();
            if (f.getName().equals(name)) return f;
        }
        return null;
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
