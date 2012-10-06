package com.example.interfaces;

import java.io.IOException;
import java.io.OutputStream;
import java.util.UUID;

import android.os.Bundle;
import android.annotation.TargetApi;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Toast;
import android.widget.Button;
import android.widget.ToggleButton;
import android.util.Log;


public class MainActivity extends Activity
{
	  private ToggleButton Power, MUTE;
	  private Button CHDW,CHUP,VUP,VDW,PRCH,B_0,B_1,B_2,B_3,B_4,B_5,B_6,B_7,B_8,B_9;
	  private String Television = "Sony";
	  private String last_cmd = "";
	  
////////////////////
//****BluShit****//
//////////////////
	  
	  private static final String TAG = "THINBTCLIENT";
      private static final boolean D = true;
      private BluetoothAdapter mBluetoothAdapter = null;
      private BluetoothSocket btSocket = null;
      private OutputStream outStream = null;
      
      // Well known SPP UUID (will *probably* map to
      // RFCOMM channel 1 (default) if not in use);
      // see comments in onResume().
      private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

      // ==> hardcode your server's MAC address here <==
      private static String address = "00:12:08:01:93:26";
      
////////////////////////
//****BluShit_END****//
//////////////////////
      
    @Override
    public void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
    	Power = (ToggleButton) findViewById(R.id.Power);
    	MUTE = (ToggleButton) findViewById(R.id.MUTE);
    	CHUP = (Button) findViewById(R.id.CHUP);
    	CHDW = (Button) findViewById(R.id.CHDW);
    	VUP = (Button) findViewById(R.id.VUP);
    	VDW = (Button) findViewById(R.id.VDW);
    	PRCH= (Button) findViewById(R.id.Prev_CH);
    	B_0 = (Button) findViewById(R.id.B_0);
    	B_1 = (Button) findViewById(R.id.B_1);
    	B_2 = (Button) findViewById(R.id.B_2);
    	B_3 = (Button) findViewById(R.id.B_3);
    	B_4 = (Button) findViewById(R.id.B_4);
    	B_5 = (Button) findViewById(R.id.B_5);
    	B_6 = (Button) findViewById(R.id.B_6);
    	B_7 = (Button) findViewById(R.id.B_7);
    	B_8 = (Button) findViewById(R.id.B_8);
    	B_9 = (Button) findViewById(R.id.B_9);
        
        //Listener de botones
        //addListenerOnButton();
        
	    if (D)
	            Log.e(TAG, "+++ ON CREATE +++");
	
	    // Get Buethooth Adapter so we can work with it latter
	    mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

	    // Check Bluetooth is available on the device
	    if (mBluetoothAdapter == null) {
	            Toast.makeText(this, 
	                    "Bluetooth is not available.", 
	                    Toast.LENGTH_LONG).show();
	            finish();
	            return;
	    }
	
	    // Check Bluetooth is enabled
	    if (!mBluetoothAdapter.isEnabled()) {
	            Toast.makeText(this, 
	                    "Please enable your BT and re-run this program.", 
	                    Toast.LENGTH_LONG).show();
	            finish();
	            return;
	    }
	
	    // Cookies =)
	    if (D)
	            Log.e(TAG, "+++ DONE IN ON CREATE, GOT LOCAL BT ADAPTER +++");
    }

    //Metodos de Banderas de programas onstart, on kill, etc..
    public void onStart() {
        super.onStart();
        if (D)
                Log.e(TAG, "++ ON START ++");
    }

@TargetApi(14) @Override
	public void onResume() {
        super.onResume();

        if (D) {
                Log.e(TAG, "+ ON RESUME +");
                Log.e(TAG, "+ ABOUT TO ATTEMPT CLIENT CONNECT +");
        }

        // When this returns, it will 'know' about the server,
        // via it's MAC address.
        BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);

        // We need two things before we can successfully connect
        // (authentication issues aside): a MAC address, which we
        // already have, and an RFCOMM channel.
        // Because RFCOMM channels (aka ports) are limited in
        // number, Android doesn't allow you to use them directly;
        // instead you request a RFCOMM mapping based on a service
        // ID. In our case, we will use the well-known SPP Service
        // ID. This ID is in UUID (GUID to you Microsofties)
        // format. Given the UUID, Android will handle the
        // mapping for you. Generally, this will return RFCOMM 1,
        // but not always; it depends what other BlueTooth services
        // are in use on your Android device.
        try {
        		Log.e(TAG, "+ Creamdp SOCKET TIO +");
                btSocket = device.createRfcommSocketToServiceRecord(MY_UUID);
                Log.e(TAG, ""+btSocket.isConnected());
        } catch (IOException e) {
                Log.e(TAG, "ON RESUME: Socket creation failed.", e);
        }

        // Discovery may be going on, e.g., if you're running a
        // 'scan for devices' search from your handset's Bluetooth
        // settings, so we call cancelDiscovery(). It doesn't hurt
        // to call it, but it might hurt not to... discovery is a
        // heavyweight process; you don't want it in progress when
        // a connection attempt is made.
        mBluetoothAdapter.cancelDiscovery();

        // Blocking connect, for a simple client nothing else can
        // happen until a successful connection is made, so we
        // don't care if it blocks.
        try {
                btSocket.connect();
                Log.e(TAG, "ON RESUME: BT connection established, data transfer link open.");
                
        } catch (IOException e) {
                try {
                        btSocket.close();
                } catch (IOException e2) {
                        Log.e(TAG, 
                                "ON RESUME: Unable to close socket during connection failure", e2);
                }
        }

        // Create a data stream so we can talk to server.
        if (D)
                Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");

        try {
                outStream = btSocket.getOutputStream();
        } catch (IOException e) {
                Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
        }
}
    
@Override
	public void onPause() {
        super.onPause();

        if (D)
                Log.e(TAG, "- ON PAUSE -");

        if (outStream != null) {
                try {
                        outStream.flush();
                } catch (IOException e) {
                        Log.e(TAG, "ON PAUSE: Couldn't flush output stream.", e);
                }
        }

        try     {
                btSocket.close();
        } catch (IOException e2) {
                Log.e(TAG, "ON PAUSE: Unable to close socket.", e2);
        }
}

@Override
	public void onStop() {
        super.onStop();
        if (D)
                Log.e(TAG, "-- ON STOP --");
}

@Override
	public void onDestroy() {
        super.onDestroy();
        if (D)
                Log.e(TAG, "--- ON DESTROY ---");
}
    
	public void sendCommand(int id){
		String cmd;
		
		// Initialize everything
		TVCommand TVCommander = new TVCommand();
		
		// Retrieve the IR Command
		cmd = TVCommander.getCode(Television, id);
		cmd = cmd + "\r\n";
		
		// Send the command to the BT Terminal
		Log.e("Command", cmd);	
		BT_sendString(cmd);
		
		if (cmd != last_cmd){
			for(int i=0; i<2; i++){
				// Wait until first comand was send
				try {
					Thread.sleep(75);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				
				// Send again the command
				BT_sendString(cmd);
			}
		}
		
		last_cmd = cmd;
	}
	
	public void BT_sendString(String message){
		byte[] msgBuffer = message.getBytes();
				
		if (D)
            Log.e(TAG, "+ Sending data to terminal ... +");
		
		// Try to open the BT Stream
	    try {
	        outStream = btSocket.getOutputStream();
	    } 
	    catch (IOException e){
	        Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
	    }
    
	    // Write the data to the BT Terminal
	    try{
            outStream.write(msgBuffer);
	    } 
	    catch (IOException e) {
	       Log.e(TAG, "ON RESUME: Exception during write.", e);
	    }
		
	}
    
	public void onTVSelection(View view) {
	    
	    // Check which radio button was clicked
	    switch(view.getId()) {
	    
	    	// Sony Television
	        case R.id.TVSelect1:
	        	Log.e(TAG, "Sony is selected");
	        	Television = "Sony";
	            break;
	            
	        // Other television =)
	        case R.id.TVSelect2:
	        	Log.e(TAG, "Other is selected");
	        	Television = "NEC";
	            break;
	    }
	}
	
	public void onRemoteButton(View view) {
		Log.e("TV_IN_USE", Television);
	    
	    // Check which button was clicked
	    switch(view.getId()) {
	    
	    	// Function Buttons
	        case R.id.Power:
	        	sendCommand(Keypad.POWER);
	            break;
	            
	        case R.id.MUTE:
	        	sendCommand(Keypad.MUTE);
	            break;
	            
	        case R.id.CHUP:
	        	sendCommand(Keypad.CH_UP);
	            break;
	            
	        case R.id.CHDW:
	        	sendCommand(Keypad.CH_DW);
	            break;
	            
	        case R.id.VUP:
	        	sendCommand(Keypad.VOL_UP);
	            break;
	            
	        case R.id.VDW:
	        	sendCommand(Keypad.VOL_DW);
	            break;
	            
	        case R.id.Prev_CH:
	        	sendCommand(Keypad.PR_CH);
	            break;
	          
	            
	        // Numeric Keypad
	        case R.id.B_0:
	        	sendCommand(Keypad.KEY_0);
	        	break;
	        	
	        case R.id.B_1:
	        	sendCommand(Keypad.KEY_1);
	        	break;
	        	
	        case R.id.B_2:
	        	sendCommand(Keypad.KEY_2);
	        	break;
	        
	        case R.id.B_3:
	        	sendCommand(Keypad.KEY_3);
	        	break;
	        	
	        case R.id.B_4:
	        	sendCommand(Keypad.KEY_4);
	        	break;
	        	
	        case R.id.B_5:
	        	sendCommand(Keypad.KEY_5);
	        	break;
	        	
	        case R.id.B_6:
	        	sendCommand(Keypad.KEY_6);
	        	break;
	        	
	        case R.id.B_7:
	        	sendCommand(Keypad.KEY_7);
	        	break;
	        	
	        case R.id.B_8:
	        	sendCommand(Keypad.KEY_8);
	        	break;
	        	
	        case R.id.B_9:
	        	sendCommand(Keypad.KEY_9);
	        	break;

	    }
	}
}
