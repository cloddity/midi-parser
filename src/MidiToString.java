import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;

public class MidiToString {
	public static void main(String[] args) {
		Map<String, Integer> notePair = new HashMap<String, Integer>() {{
			put("D5", 1);
			put("C#5", 5);
			put("C5", 10);
			put("B4", 14);
			put("A#4", 19);
		    put("A4", 24);
		    put("G#4", 29);
		    put("G4", 35);
		    put("F#4", 40);
		    put("F4", 46);
		    put("E4", 52);
		    put("D#4", 59);
		    put("D4", 66);
		    put("C#4", 73);
		    put("C4", 80);
		    put("B3", 89);
		    put("A#3", 97);
		    put("A3", 105);
		    put("G#3", 115);
		    put("G3", 126);
		    put("F#3", 137);
		    put("F3", 149);
		    put("E3", 161);
		    put("D#3", 174);
		    put("D3", 188);
		    put("C#3", 200);
		}};
		
		try {
			ArrayList<Integer> startList = new ArrayList<Integer>(); // 0, 100, 200...
			ArrayList<String> noteList = new ArrayList<String>(); // 0-200
			ArrayList<Integer> endList = new ArrayList<Integer>(); // 100, 200, 300...
			
			Scanner scan = new Scanner(System.in);
			Scanner read; 
			int measure, beat, note, length, begin, next, count;
			measure = beat = note = length = begin = next = count = 0;
			int factor = 5;
			
			System.out.print("Interval: ");
			int n = Integer.parseInt(scan.nextLine());
			System.out.print("BPM: ");
			int bpm = Integer.parseInt(scan.nextLine());
			System.out.print("Filepath: ");
			String filePath = scan.nextLine();
			if (filePath.contains("\""))
				filePath = filePath.substring(1, filePath.length() - 1);
			String contents = new Scanner(new File(filePath)).useDelimiter("\\Z").next().replaceAll(" ", "");
			read = new Scanner(contents);
			
			//while (read.hasNextLine()) {
				//count++;
			//}
			//read.close();
			//read = new Scanner(contents);
			
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
			
			int[] start, end, endClone, endMain, vib, notes;
			start = new int[endList.size()];
			end = new int[endList.size()];
			endClone = new int[endList.size()];
			endMain = new int[endList.size()];
			vib = new int[endList.size()];
			notes = new int[endList.size()];
			startList.add(endList.get(endList.size() - 1));
			
			double midiToMs = (60.0 / bpm) * 1000 / n;
			
			for(int i = 0; i < endList.size(); i++) {
				//begin = (int) ((startList.get(i) - startList.get(0)) * midiToMs);  // 0, 1000, 2000...
				length = (int) ((endList.get(i) - startList.get(0)) * midiToMs);   // 1000, 2000, 3000...
				//next = (int) ((startList.get(i + 1) - startList.get(i)) * midiToMs);  // 1000, 1000...
				//if (length > next)
					//length = next;
				int pitch = notePair.get(noteList.get(i));
				notes[i] = pitch;
				//System.out.println("play(" + pitch + ", " + length + ", " + next + ");");
				//int vibrato = length;
				if (i == 0) {
					start[i] = 0;
					end[i] = length / factor;
					endClone[i] = length;
				}
				else {
					start[i] = (int) ((startList.get(i) - startList.get(i - 1)) * midiToMs); 
					end[i] = (int) ((endList.get(i) - endList.get(i - 1)) * midiToMs / factor); 
					endClone[i] = (int) ((endList.get(i) - endList.get(i - 1)) * midiToMs);
					//if (i % 2 == 0)
						//end[i] += 1;
				}
				//len[i] = length;
				//if ((len[i] - start[i]) > 400)
					//vibrato = len[i] - (len[i] - start[i]) / 2;
				//vib[i] = vibrato;
				
				//System.out.println("playV(" + pitch + ", " + length + ", " + vibrato + ", 2);");
				//System.out.println("silence(" + length + ");");
			}
			
			int diff = 0;
			
			for (int i = 0; i < endList.size(); i++) {
				endMain[i] = factor * end[i];
			}
			
			for (int i = 0; i < endList.size(); i++) {
				diff += endClone[i] - endMain[i];
			}
			
			for (int i = 1; i < (diff / factor); i++) { // timing correction
				end[i * factor] += 1;
			}
			
			//System.out.println(diff / factor);
			System.out.println("int arrSize = " + endList.size() + ";");
			System.out.println("byte notes[] = {" + Arrays.toString(notes).substring(1, Arrays.toString(notes).length() - 1) + "};");
			System.out.println("int start[] = {" + Arrays.toString(start).substring(1, Arrays.toString(start).length() - 1) + "};");
			System.out.println("byte finish[] = {" + Arrays.toString(end).substring(1, Arrays.toString(end).length() - 1) + "};");
			System.out.println("byte factor = " + factor + ";");
			//System.out.println("int vib[] = {" + Arrays.toString(vib).substring(1, Arrays.toString(vib).length() - 1) + "};");
		}
		catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}
}
