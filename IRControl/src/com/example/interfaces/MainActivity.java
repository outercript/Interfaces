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
import android.widget.RadioGroup;
import android.widget.Toast;
import android.widget.Button;
import android.widget.ToggleButton;
import android.util.Log;


public class MainActivity extends Activity 
{
	  private ToggleButton Power, MUTE;
	  private Button CHDW,CHUP,VUP,VDW,B_0,B_1,B_2,B_3,B_4,B_5,B_6,B_7,B_8,B_9;
	  
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
      private static final UUID MY_UUID = 
                      UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

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
        
        //Listener de botones
        addListenerOnButton();
        
        
		    if (D)
		            Log.e(TAG, "+++ ON CREATE +++");
		
		    mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
		    if (mBluetoothAdapter == null) {
		            Toast.makeText(this, 
		                    "Bluetooth is not available.", 
		                    Toast.LENGTH_LONG).show();
		            finish();
		            return;
		    }
		
		    if (!mBluetoothAdapter.isEnabled()) {
		            Toast.makeText(this, 
		                    "Please enable your BT and re-run this program.", 
		                    Toast.LENGTH_LONG).show();
		            finish();
		            return;
		    }
		
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
        //Hello message to start conversation.//
        /*/String message = "Hello message from client to server.\r\n";
       // byte[] msgBuffer = message.getBytes();
        try {
               // outStream.write(msgBuffer);
        } catch (IOException e) {
                Log.e(TAG, "ON RESUME: Exception during write.", e);
        }*/
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
    
   //codigo para los toggle button /////Display solo imprime los textos de lostoggle botones
    public void addListenerOnButton() 
    {
    	Power = (ToggleButton) findViewById(R.id.Power);
    	MUTE = (ToggleButton) findViewById(R.id.MUTE);
    	CHUP = (Button) findViewById(R.id.CHUP);
    	CHDW = (Button) findViewById(R.id.CHDW);
    	VUP = (Button) findViewById(R.id.VUP);
    	VDW = (Button) findViewById(R.id.VDW);
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
    	
    	//Codigo para ENVIAR POWER SINGAL
    	Power.setOnClickListener(new OnClickListener()
    	{
    		public void onClick(View v)
    		{	
    			RadioGroup radioGroup = (RadioGroup) findViewById(R.id.radioGroup1);
        		int checkedRadioButton = radioGroup.getCheckedRadioButtonId();
        		String radioButtonSelected = ""; 
        		switch (checkedRadioButton)
        		{
        		  case R.id.TVSelect1 : radioButtonSelected = "SONY";                	              
        		  						break;
        		  case R.id.TVSelect2: radioButtonSelected = "Samsung";
        				                break;		                      
        		}
        		//////////////////////////////////////////////////////////
        		///***Identificacion del radio button Seleccionado***/////
        		/////////////////////////////////////////////////////////
        		
        		if(radioButtonSelected == "SONY")
        		{
    	    		if (D)
    		                Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
    		        try {
    		                outStream = btSocket.getOutputStream();
    		        } catch (IOException e) 
    		        {
    		                Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
    		        }
    		
    		        String message = "SA90\r\n";
    		        byte[] msgBuffer = message.getBytes();
    		        try {
    		                outStream.write(msgBuffer);
    		               // outStream.flush();
    		        } catch (IOException e) 
    		        {
    		                Log.e(TAG, "ON RESUME: Exception during write.", e);
    		        }
        		}
        		
        		if(radioButtonSelected == "Samsung")
        		{
    	    		if (D)
    		                Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
    		        try {
    		                outStream = btSocket.getOutputStream();
    		        } catch (IOException e) 
    		        {
    		                Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
    		        }
    		
    		        String message = "RA90\r\n";
    		        byte[] msgBuffer = message.getBytes();
    		        try {
    		                outStream.write(msgBuffer);
    		        } catch (IOException e) 
    		        {
    		                Log.e(TAG, "ON RESUME: Exception during write.", e);
    		        }
        		} 
    		}
    	});
    	
    	//Codigo para MUTE ON OFF
    	MUTE.setOnClickListener(new OnClickListener()
    	{
    		public void onClick(View v)
    		{
    			RadioGroup radioGroup = (RadioGroup) findViewById(R.id.radioGroup1);
        		int checkedRadioButton = radioGroup.getCheckedRadioButtonId();
        		String radioButtonSelected = ""; 
        		switch (checkedRadioButton)
        		{
        		  case R.id.TVSelect1 : radioButtonSelected = "SONY";                	              
        		  						break;
        		  case R.id.TVSelect2: radioButtonSelected = "Samsung";
        				                break;		                      
        		}
        		
    			//////////////////////////////////////////////////////////
    			///***Identificacion del radio button Seleccionado***/////
    			/////////////////////////////////////////////////////////

    			if(radioButtonSelected == "SONY")
    			{
    				if (D)
    					Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
    				try {
    					outStream = btSocket.getOutputStream();
    				} catch (IOException e) 
    				{
    					Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
    				}

    				String message = "S290\r\n";
    				byte[] msgBuffer = message.getBytes();
    				try {
    					outStream.write(msgBuffer);
    					// outStream.flush();
    				} catch (IOException e) 
    				{
    					Log.e(TAG, "ON RESUME: Exception during write.", e);
    				}
    			}

    			if(radioButtonSelected == "Samsung")
    			{
    				if (D)
    					Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
    				try {
    					outStream = btSocket.getOutputStream();
    				} catch (IOException e) 
    				{
    					Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
    				}

    				String message = "R290\r\n";
    				byte[] msgBuffer = message.getBytes();
    				try {
    					outStream.write(msgBuffer);
    					// outStream.flush();
    				} catch (IOException e) 
    				{
    					Log.e(TAG, "ON RESUME: Exception during write.", e);
    				}
    			}	
    		}
    	});
    	
/*******************************************************************************************************************************************************************************************
 * AQUI SE EMPEZO A PEGAR DEL EXCEL SOBREESCRIBIENDO COSOS
 * 
 * ****************************************************************************************************************************************************************************************/
		B_1.setOnClickListener(new OnClickListener()
			{
			   public void onClick(View v)
	      { 
	       RadioGroup radioGroup = (RadioGroup) findViewById(R.id.radioGroup1);
	          int checkedRadioButton = radioGroup.getCheckedRadioButtonId();
	          String radioButtonSelected = ""; 
	          switch (checkedRadioButton)
	          {
	            case R.id.TVSelect1 : radioButtonSelected = "Sony";                               
	                  break;
	
	            case R.id.TVSelect2 : radioButtonSelected = "Samsung";                               
	                  break;
	                        
	          }
	          //////////////////////////////////////////////////////////
	          ///***Identificacion del radio button Seleccionado***/////
	          /////////////////////////////////////////////////////////
	          
	          if(radioButtonSelected == "Sony")
	          {
	           if (D)
	                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
	              try {
	                      outStream = btSocket.getOutputStream();
	              } catch (IOException e) 
	              {
	                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
	              }
	      
	              String message = "S010\r\n";
	              byte[] msgBuffer = message.getBytes();
	              try {
	                      outStream.write(msgBuffer);
	                     // outStream.flush();
	              } catch (IOException e) 
	              {
	                      Log.e(TAG, "ON RESUME: Exception during write.", e);
	              }
	          }
	          
	          
	          if(radioButtonSelected == "Samsung")
	          {
	           if (D)
	                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
	              try {
	                      outStream = btSocket.getOutputStream();
	              } catch (IOException e) 
	              {
	                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
	              }
	      
	              String message = "R010\r\n";
	              byte[] msgBuffer = message.getBytes();
	              try {
	                      outStream.write(msgBuffer);
	                     // outStream.flush();
	              } catch (IOException e) 
	              {
	                      Log.e(TAG, "ON RESUME: Exception during write.", e);
	              }
	          }
	           
	      }
			});
		B_2.setOnClickListener(new OnClickListener()
		     {
		      public void onClick(View v)
		      { 
		       RadioGroup radioGroup = (RadioGroup) findViewById(R.id.radioGroup1);
		          int checkedRadioButton = radioGroup.getCheckedRadioButtonId();
		          String radioButtonSelected = ""; 
		          switch (checkedRadioButton)
		          {
		            case R.id.TVSelect1 : radioButtonSelected = "Sony";                               
		                  break;
		
		            case R.id.TVSelect2 : radioButtonSelected = "Samsung";                               
		                  break;
		                        
		          }
		          //////////////////////////////////////////////////////////
		          ///***Identificacion del radio button Seleccionado***/////
		          /////////////////////////////////////////////////////////
		          
		          if(radioButtonSelected == "Sony")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "S810\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		          
		          
		          if(radioButtonSelected == "Samsung")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "R810\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		           
		      }
		     });
		B_3.setOnClickListener(new OnClickListener()
		     {
		      public void onClick(View v)
		      { 
		       RadioGroup radioGroup = (RadioGroup) findViewById(R.id.radioGroup1);
		          int checkedRadioButton = radioGroup.getCheckedRadioButtonId();
		          String radioButtonSelected = ""; 
		          switch (checkedRadioButton)
		          {
		            case R.id.TVSelect1 : radioButtonSelected = "Sony";                               
		                  break;
		
		            case R.id.TVSelect2 : radioButtonSelected = "Samsung";                               
		                  break;
		                        
		          }
		          //////////////////////////////////////////////////////////
		          ///***Identificacion del radio button Seleccionado***/////
		          /////////////////////////////////////////////////////////
		          
		          if(radioButtonSelected == "Sony")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "S410\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		          
		          
		          if(radioButtonSelected == "Samsung")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "R410\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		           
		      }
		     });
		B_4.setOnClickListener(new OnClickListener()
		     {
		      public void onClick(View v)
		      { 
		       RadioGroup radioGroup = (RadioGroup) findViewById(R.id.radioGroup1);
		          int checkedRadioButton = radioGroup.getCheckedRadioButtonId();
		          String radioButtonSelected = ""; 
		          switch (checkedRadioButton)
		          {
		            case R.id.TVSelect1 : radioButtonSelected = "Sony";                               
		                  break;
		
		            case R.id.TVSelect2 : radioButtonSelected = "Samsung";                               
		                  break;
		                        
		          }
		          //////////////////////////////////////////////////////////
		          ///***Identificacion del radio button Seleccionado***/////
		          /////////////////////////////////////////////////////////
		          
		          if(radioButtonSelected == "Sony")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "SC10\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		          
		          
		          if(radioButtonSelected == "Samsung")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "RC11\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		           
		      }
		     });
		B_5.setOnClickListener(new OnClickListener()
		     {
		      public void onClick(View v)
		      { 
		       RadioGroup radioGroup = (RadioGroup) findViewById(R.id.radioGroup1);
		          int checkedRadioButton = radioGroup.getCheckedRadioButtonId();
		          String radioButtonSelected = ""; 
		          switch (checkedRadioButton)
		          {
		            case R.id.TVSelect1 : radioButtonSelected = "Sony";                               
		                  break;
		
		            case R.id.TVSelect2 : radioButtonSelected = "Samsung";                               
		                  break;
		                        
		          }
		          //////////////////////////////////////////////////////////
		          ///***Identificacion del radio button Seleccionado***/////
		          /////////////////////////////////////////////////////////
		          
		          if(radioButtonSelected == "Sony")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "S210\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		          
		          
		          if(radioButtonSelected == "Samsung")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "R211\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		           
		      }
		     });
		B_6.setOnClickListener(new OnClickListener()
		     {
		      public void onClick(View v)
		      { 
		       RadioGroup radioGroup = (RadioGroup) findViewById(R.id.radioGroup1);
		          int checkedRadioButton = radioGroup.getCheckedRadioButtonId();
		          String radioButtonSelected = ""; 
		          switch (checkedRadioButton)
		          {
		            case R.id.TVSelect1 : radioButtonSelected = "Sony";                               
		                  break;
		
		            case R.id.TVSelect2 : radioButtonSelected = "Samsung";                               
		                  break;
		                        
		          }
		          //////////////////////////////////////////////////////////
		          ///***Identificacion del radio button Seleccionado***/////
		          /////////////////////////////////////////////////////////
		          
		          if(radioButtonSelected == "Sony")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "SA10\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		          
		          
		          if(radioButtonSelected == "Samsung")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "RA11\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		           
		      }
		     });
		B_7.setOnClickListener(new OnClickListener()
		     {
		      public void onClick(View v)
		      { 
		       RadioGroup radioGroup = (RadioGroup) findViewById(R.id.radioGroup1);
		          int checkedRadioButton = radioGroup.getCheckedRadioButtonId();
		          String radioButtonSelected = ""; 
		          switch (checkedRadioButton)
		          {
		            case R.id.TVSelect1 : radioButtonSelected = "Sony";                               
		                  break;
		
		            case R.id.TVSelect2 : radioButtonSelected = "Samsung";                               
		                  break;
		                        
		          }
		          //////////////////////////////////////////////////////////
		          ///***Identificacion del radio button Seleccionado***/////
		          /////////////////////////////////////////////////////////
		          
		          if(radioButtonSelected == "Sony")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "S610\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		          
		          
		          if(radioButtonSelected == "Samsung")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "R611\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		           
		      }
		     });
		B_8.setOnClickListener(new OnClickListener()
		     {
		      public void onClick(View v)
		      { 
		       RadioGroup radioGroup = (RadioGroup) findViewById(R.id.radioGroup1);
		          int checkedRadioButton = radioGroup.getCheckedRadioButtonId();
		          String radioButtonSelected = ""; 
		          switch (checkedRadioButton)
		          {
		            case R.id.TVSelect1 : radioButtonSelected = "Sony";                               
		                  break;
		
		            case R.id.TVSelect2 : radioButtonSelected = "Samsung";                               
		                  break;
		                        
		          }
		          //////////////////////////////////////////////////////////
		          ///***Identificacion del radio button Seleccionado***/////
		          /////////////////////////////////////////////////////////
		          
		          if(radioButtonSelected == "Sony")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "SE10\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		          
		          
		          if(radioButtonSelected == "Samsung")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "RE11\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		           
		      }
		     });
		B_9.setOnClickListener(new OnClickListener()
		     {
		      public void onClick(View v)
		      { 
		       RadioGroup radioGroup = (RadioGroup) findViewById(R.id.radioGroup1);
		          int checkedRadioButton = radioGroup.getCheckedRadioButtonId();
		          String radioButtonSelected = ""; 
		          switch (checkedRadioButton)
		          {
		            case R.id.TVSelect1 : radioButtonSelected = "Sony";                               
		                  break;
		
		            case R.id.TVSelect2 : radioButtonSelected = "Samsung";                               
		                  break;
		                        
		          }
		          //////////////////////////////////////////////////////////
		          ///***Identificacion del radio button Seleccionado***/////
		          /////////////////////////////////////////////////////////
		          
		          if(radioButtonSelected == "Sony")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "S110\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		          
		          
		          if(radioButtonSelected == "Samsung")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "R110\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		           
		      }
		     });
		B_0.setOnClickListener(new OnClickListener()
		     {
		      public void onClick(View v)
		      { 
		       RadioGroup radioGroup = (RadioGroup) findViewById(R.id.radioGroup1);
		          int checkedRadioButton = radioGroup.getCheckedRadioButtonId();
		          String radioButtonSelected = ""; 
		          switch (checkedRadioButton)
		          {
		            case R.id.TVSelect1 : radioButtonSelected = "Sony";                               
		                  break;
		
		            case R.id.TVSelect2 : radioButtonSelected = "Samsung";                               
		                  break;
		                        
		          }
		          //////////////////////////////////////////////////////////
		          ///***Identificacion del radio button Seleccionado***/////
		          /////////////////////////////////////////////////////////
		          
		          if(radioButtonSelected == "Sony")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "S910\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		          
		          
		          if(radioButtonSelected == "Samsung")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "R910\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		           
		      }
		     });
		CHUP.setOnClickListener(new OnClickListener()
		     {
		      public void onClick(View v)
		      { 
		       RadioGroup radioGroup = (RadioGroup) findViewById(R.id.radioGroup1);
		          int checkedRadioButton = radioGroup.getCheckedRadioButtonId();
		          String radioButtonSelected = ""; 
		          switch (checkedRadioButton)
		          {
		            case R.id.TVSelect1 : radioButtonSelected = "Sony";                               
		                  break;
		
		            case R.id.TVSelect2 : radioButtonSelected = "Samsung";                               
		                  break;
		                        
		          }
		          //////////////////////////////////////////////////////////
		          ///***Identificacion del radio button Seleccionado***/////
		          /////////////////////////////////////////////////////////
		          
		          if(radioButtonSelected == "Sony")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "S090\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		          
		          
		          if(radioButtonSelected == "Samsung")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "R090\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		           
		      }
		     });
		CHDW.setOnClickListener(new OnClickListener()
		     {
		      public void onClick(View v)
		      { 
		       RadioGroup radioGroup = (RadioGroup) findViewById(R.id.radioGroup1);
		          int checkedRadioButton = radioGroup.getCheckedRadioButtonId();
		          String radioButtonSelected = ""; 
		          switch (checkedRadioButton)
		          {
		            case R.id.TVSelect1 : radioButtonSelected = "Sony";                               
		                  break;
		
		            case R.id.TVSelect2 : radioButtonSelected = "Samsung";                               
		                  break;
		                        
		          }
		          //////////////////////////////////////////////////////////
		          ///***Identificacion del radio button Seleccionado***/////
		          /////////////////////////////////////////////////////////
		          
		          if(radioButtonSelected == "Sony")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "S890\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		          
		          
		          if(radioButtonSelected == "Samsung")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "R890\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		           
		      }
		     });
		VUP.setOnClickListener(new OnClickListener()
		     {
		      public void onClick(View v)
		      { 
		       RadioGroup radioGroup = (RadioGroup) findViewById(R.id.radioGroup1);
		          int checkedRadioButton = radioGroup.getCheckedRadioButtonId();
		          String radioButtonSelected = ""; 
		          switch (checkedRadioButton)
		          {
		            case R.id.TVSelect1 : radioButtonSelected = "Sony";                               
		                  break;
		
		            case R.id.TVSelect2 : radioButtonSelected = "Samsung";                               
		                  break;
		                        
		          }
		          //////////////////////////////////////////////////////////
		          ///***Identificacion del radio button Seleccionado***/////
		          /////////////////////////////////////////////////////////
		          
		          if(radioButtonSelected == "Sony")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "S490\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		          
		          
		          if(radioButtonSelected == "Samsung")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "R490\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		           
		      }
		     });
		VDW.setOnClickListener(new OnClickListener()
		     {
		      public void onClick(View v)
		      { 
		       RadioGroup radioGroup = (RadioGroup) findViewById(R.id.radioGroup1);
		          int checkedRadioButton = radioGroup.getCheckedRadioButtonId();
		          String radioButtonSelected = ""; 
		          switch (checkedRadioButton)
		          {
		            case R.id.TVSelect1 : radioButtonSelected = "Sony";                               
		                  break;
		
		            case R.id.TVSelect2 : radioButtonSelected = "Samsung";                               
		                  break;
		                        
		          }
		          //////////////////////////////////////////////////////////
		          ///***Identificacion del radio button Seleccionado***/////
		          /////////////////////////////////////////////////////////
		          
		          if(radioButtonSelected == "Sony")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "SC90\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		          
		          
		          if(radioButtonSelected == "Samsung")
		          {
		           if (D)
		                      Log.e(TAG, "+ ABOUT TO SAY SOMETHING TO SERVER +");
		              try {
		                      outStream = btSocket.getOutputStream();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Output stream creation failed.", e);
		              }
		      
		              String message = "RC91\r\n";
		              byte[] msgBuffer = message.getBytes();
		              try {
		                      outStream.write(msgBuffer);
		                     // outStream.flush();
		              } catch (IOException e) 
		              {
		                      Log.e(TAG, "ON RESUME: Exception during write.", e);
		              }
		          }
		           
		      }
		     });


/***************************************************************************************************************************************************
 * 
 *                            FIN
 * 
 * 
 ***************************************************************************************************************************************************/

    
    }
}
