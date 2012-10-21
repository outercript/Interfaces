package com.example.interfaces;

import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.Toast;


public class CustomOnItemSelectedListener extends MainActivity implements OnItemSelectedListener  {
	
	public MainActivity parent;
	
	public CustomOnItemSelectedListener(MainActivity dummy){
		super();
		this.parent = dummy;
	}
	
	public void onItemSelected(AdapterView<?> parent, View view, int pos,long id) {
		
			String value;
			int tmp;		
			//Get the String of the Edit Text.
			 value = parent.getItemAtPosition(pos).toString();
			 
			 // Iterate through each button values
			 for(int i = 0; i<value.length(); i++){
				 
				 // Parse the int in the current index, so that it matches the number keypad to send the command
				 
				 if(value.charAt(i) >= '0' && value.charAt(i) <= '9'){
					 tmp =value.charAt(i)-48;
					 this.parent.sendCommand(tmp);
				 }
				 else
					 break;
				 
				 Log.e("Manuel","Index : " + i + " - Button : " + tmp + " "); 
			 }
			 
	  }
	 
	public void onNothingSelected(AdapterView<?> arg0) {
		// TODO Auto-generated method stub
	  }

}
