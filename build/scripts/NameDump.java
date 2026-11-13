// NameDump.java - rename known functions from map file, then decompile ALL
// functions to <outdir>/<name>.c plus index.csv. args: <mapfile> <outdir>
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionManager;
import ghidra.program.model.symbol.SourceType;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;

public class NameDump extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] a = getScriptArgs();
        Path mapf = Paths.get(a[0]), out = Paths.get(a[1]);
        Files.createDirectories(out);
        FunctionManager fm = currentProgram.getFunctionManager();

        // 1) naming pass
        int renamed = 0;
        for (String line : Files.readAllLines(mapf, StandardCharsets.UTF_8)) {
            line = line.trim();
            if (line.isEmpty() || line.startsWith("#")) continue;
            String[] p = line.split("\\|");
            long addr = Long.parseLong(p[0].replace("0x", ""), 16);
            Function f = fm.getFunctionAt(toAddr(addr));
            if (f == null) { println("NOMATCH " + line); continue; }
            String old = f.getName();
            if (!old.equals(p[1])) {
                f.setName(p[1], SourceType.USER_DEFINED);
                renamed++;
            }
            if (p.length > 2 && !p[2].isEmpty()) f.setComment(p[2]);
            println("RENAMED " + Long.toHexString(addr) + " " + old + " -> " + p[1]);
        }
        println("renamed=" + renamed);

        // 2) dump all
        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);
        try (PrintWriter idx = new PrintWriter(
                Files.newBufferedWriter(out.resolve("index.csv")))) {
            idx.println("name,entry,signature");
            int ok = 0, bad = 0;
            for (Function f : fm.getFunctions(true)) {
                String name = f.getName();
                DecompileResults res = decomp.decompileFunction(f, 60, monitor);
                if (res.decompileCompleted()) {
                    Files.writeString(out.resolve(name + ".c"),
                        res.getDecompiledFunction().getC(), StandardCharsets.UTF_8);
                    ok++;
                } else {
                    Files.writeString(out.resolve(name + ".c"),
                        "/* DECOMPILE FAILED: " + res.getErrorMessage() + " */\n",
                        StandardCharsets.UTF_8);
                    bad++;
                }
                idx.println(name + "," + f.getEntryPoint() + ",\"" +
                    f.getSignature().getPrototypeString(false).replace("\"", "'") + "\"");
            }
            println("dumped ok=" + ok + " failed=" + bad);
        }
        decomp.dispose();
    }
}
