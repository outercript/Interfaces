package com.example.interfaces;

public class TVCommand{
	String [] Sony = new String[16];
	String [] Samsung = new String[16];
	
	public TVCommand(){
		setup_Sony();
	}
	
	public void setup_Sony(){
		// Numeric Keypad 0 to 9
		Sony[0] = "S910";
		Sony[1] = "S010";
		Sony[2] = "S810";
		Sony[3] = "S410";
		Sony[4] = "SC10";
		Sony[5] = "S210";
		Sony[6] = "SA10";
		Sony[7] = "S610";
		Sony[8] = "SE10";
		Sony[9] = "S110";
		
		// Function Buttons
		Sony[10] = "SA90"; // Power
		Sony[11] = "S290"; // Mute
		Sony[12] = "S090"; // Ch+
		Sony[13] = "S890"; // Ch-
		Sony[14] = "S490"; // Vol+
		Sony[15] = "SC90"; // Vol-
	}

	public String getCode(String TV, int command){
		String buffer = "";
		
		if (TV == "Sony"){
			buffer = Sony[command];
		}
				
		
		return buffer;
	}

}
