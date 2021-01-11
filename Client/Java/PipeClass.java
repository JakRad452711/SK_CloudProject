package testowyframes;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.concurrent.TimeUnit;



public class PipeClass {
	private static int numOfDownloads = 0;
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
	
	public int size() {
		int value = 0;
		try {
			value = (int) namedPipe.length();
		} catch (IOException e) { e.printStackTrace(); }
		
		return value;
	}
	
	public byte[] read() {
		int length = 0;
		
		while(size() != ((numOfDownloads + 1) * 4096)) {
			try { TimeUnit.MILLISECONDS.sleep(50); } 
			catch (Exception e1) { e1.printStackTrace(); }
		}
		
		byte[] line = new byte[4096];
		
		numOfDownloads ++;
		
		try {
			namedPipe.read(line); 
		} catch (IOException e) { e.printStackTrace(); }
		
		return line;
	}
	
	public void write(byte[] toWrite) {
		try { namedPipe.write(toWrite); } catch (IOException e) { e.printStackTrace(); }
	}
	
	public void destroyNamedPipe() {
		try { namedPipe.close(); } catch (IOException e) { e.printStackTrace(); }
	}
}