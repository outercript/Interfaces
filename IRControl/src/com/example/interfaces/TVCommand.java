package com.example.interfaces;

public class TVCommand{
	String [] Sony = new String[17];
	String [] NEC = new String[17];
	
	public TVCommand(){
		setup_Sony();
		setup_NEC();
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
		Sony[16] = "SC90"; // CHANGE TO SONYS PREV CHAN COMMAND********
	}
	
	public void setup_NEC(){
		// Numeric Keypad 0 to 9
		NEC[0] = "N02FD00FF";
		NEC[1] = "N02FD807F";
		NEC[2] = "N02FD40BF";
		NEC[3] = "N02FDC03F";
		NEC[4] = "N02FD20DF";
		NEC[5] = "N02FDA05F";
		NEC[6] = "N02FD609F";
		NEC[7] = "N02FDE01F";
		NEC[8] = "N02FD10EF";
		NEC[9] = "N02FD906F";
		
		// Function Buttons
		NEC[10] = "N02FD48B7"; // Power
		NEC[11] = "N02FD08F7"; // Mute
		NEC[12] = "N02FDD827"; // Ch+
		NEC[13] = "N02FDF807"; // Ch-
		NEC[14] = "N02FD58A7"; // Vol+
		NEC[15] = "N02FD7887"; // Vol-
		NEC[16] = "N02FDE817"; // PREVIOUS CHANNEL
	}

	public String getCode(String TV, int command){
		String buffer = "";
		
		if (TV == "Sony"){
			buffer = Sony[command];
		}
		else if (TV== "NEC"){
			buffer = NEC[command];
		}

		return buffer;
	}

}
