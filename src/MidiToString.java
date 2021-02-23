import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;

public class MidiToString {
	public static void main(String[] args) {
		
		Map<String, Integer> notePair = new HashMap<String, Integer>() {{
		    put("A#4", 0);
		    put("A4", 4);
		    put("G#4", 9);
		    put("G4", 15);
		    put("F#4", 20);
		    put("F4", 26);
		    put("E4", 32);
		    put("D#4", 39);
		    put("D4", 46);
		    put("C#4", 53);
		    put("C4", 60);
		    put("B3", 69);
		    put("A#3", 77);
		    put("A3", 85);
		}};
		
		try {
			ArrayList<Integer> startList = new ArrayList<Integer>();
			ArrayList<String> noteList = new ArrayList<String>();
			ArrayList<Integer> endList = new ArrayList<Integer>();
			
			Scanner scan = new Scanner(System.in);
			Scanner read; 
			int measure, beat, note;
			measure = beat = note = 0;
			
			System.out.print("Interval: ");
			int n = Integer.parseInt(scan.nextLine());
			System.out.print("BPM: ");
			int bpm = Integer.parseInt(scan.nextLine());
			System.out.print("Filepath: ");
			String filePath = scan.nextLine();
			String contents = new Scanner(new File(filePath)).useDelimiter("\\Z").next().replaceAll(" ", "");
			read = new Scanner(contents);
			
			while (read.hasNextLine()) {  // add lines into list
				String[] parse = read.nextLine().split("\\|");
				String time = parse[0];
				
				if (time.split(":").length == 1 && !time.equals("")) {
					note = Integer.parseInt(time);
				}
				else if (time.split(":").length == 2) {
					beat = Integer.parseInt(time.split(":")[0]);
					note = Integer.parseInt(time.split(":")[1]);
				}
				else if (time.split(":").length == 3) {
					measure = Integer.parseInt(time.split(":")[0]);
					beat = Integer.parseInt(time.split(":")[1]);
					note = Integer.parseInt(time.split(":")[2]);
				}
				
				if (parse[1].contains("On")) {
					noteList.add(parse[3].split("=")[1]);
					startList.add(4 * (measure - 1) * n + (beat - 1) * n + note);
				}
				else {
					endList.add(4 * (measure - 1) * n + (beat - 1) * n + note);
				}
			}
			read.close();
			
			startList.add(endList.get(endList.size() - 1));
			
			for(int i = 0; i < endList.size(); i++) {
				int length = (int) ((endList.get(i) - startList.get(i)) * (60.0 / bpm) * 1000 / n);
				int next = (int) ((startList.get(i + 1) - startList.get(i)) * (60.0 / bpm) * 1000 / n);
				if (length > next) {
					length = next;
				}
				int pitch = notePair.get(noteList.get(i));
				//System.out.println("play(" + pitch + ", " + length + ", " + next + ");");
				int vibrato = length;
				if (length > 400)
					vibrato = length / 2;
				System.out.println("playV(" + pitch + ", " + length + ", " + next + ", " + vibrato + ", 2);");
			}
		}
		catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}
}
