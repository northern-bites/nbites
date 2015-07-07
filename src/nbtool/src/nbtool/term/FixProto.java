package nbtool.term;

import java.io.File;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;

//import nbtool.util.Logger;

public class FixProto {
	
	public static void main(String[] args) throws Exception {
		String nbdir = System.getenv("NBITES_DIR");
		nbdir += "/src/share/messages/";
		System.out.println("at: " + nbdir);
		
		File mdir = new File(nbdir);
		assert(mdir.exists() && mdir.isDirectory());
		for (File file : mdir.listFiles()) {
			if (file.getAbsolutePath().endsWith(".proto")) {
				System.out.printf("found: %s\n", file.getName());
				byte[] bytes = Files.readAllBytes(file.toPath());
				String content = new String(bytes, StandardCharsets.UTF_8);
				
				//assert(!content.contains("java_"));
				int stt_i = content.indexOf("package messages;");
				int end_i = stt_i + "package messages;".length();
				if (stt_i < 0) {
					throw new Exception(content);
				}
				
				String fileName = file.getName().substring(0, file.getName().length() - ".proto".length());
				//Logger.println(fileName);
				String outerName = String.format("_File_%s", fileName);
				String start = content.substring(0, end_i);
				String end = content.substring(end_i);
				String middle = String.format("\n\noption java_outer_classname = \"%s\";\n" +
						"option java_multiple_files = true;\n\n",
						outerName);
				
				String final_content = start + middle + end;
				System.out.println(final_content);
				Files.write(file.toPath(), final_content.getBytes(StandardCharsets.UTF_8));
			}
		}
	}

}
