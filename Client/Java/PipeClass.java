package testowyframes;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;



public class PipeClass {
	String namedPipeLocation;
	RandomAccessFile namedPipe;
	
	public PipeClass(String namedPipeLocation) {
		try {
			this.namedPipeLocation = namedPipeLocation;
			namedPipe = new RandomAccessFile(namedPipeLocation, "rw");
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}
	
	public byte[] read() {
		byte[] line = new byte[4096];
		try { namedPipe.readFully(line); } catch (IOException e) { e.printStackTrace(); }
		return line;
	}
	
	public void write(byte[] toWrite) {
		try { namedPipe.write(toWrite); } catch (IOException e) { e.printStackTrace(); }
	}
	
	public void destroyNamedPipe() {
		try { namedPipe.close(); } catch (IOException e) { e.printStackTrace(); }
	}
}
