package name.osher.gil.minivmac;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.app.Activity;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.Toast;

public class CreateDisk extends Activity {
	
	static final int PROGRESS_DIALOG = 0;
    ProgressThread progressThread;
    ProgressDialog progressDialog;

	TextView sizeText;
	EditText name;
	SeekBar size;
	Button create;
	
	Activity mContext = this;
	
	@Override
    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);
    	
        setContentView(R.layout.create_disk);
        sizeText = (TextView)findViewById(R.id.sizeText);
        name = (EditText)findViewById(R.id.name);
        size = (SeekBar)findViewById(R.id.size);
        create = (Button)findViewById(R.id.create);
        
        size.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
				
			}
			
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
				
			}
			
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				String s;
				double size = progress + 400;
				if (size < 2048)
					s = String.format(" %4.0f KiB", size);
				else
					s = String.format(" %3.2f MiB", size / 1024.0);
				sizeText.setText(s);
			}
		});
        
        create.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				showDialog(PROGRESS_DIALOG);
			}
		});
        
        size.setMax(128000 - 400);
        size.setProgress(20480 - 400);
	}
   
    protected Dialog onCreateDialog(int id) {
        switch(id) {
        case PROGRESS_DIALOG:
            progressDialog = new ProgressDialog(CreateDisk.this);
            progressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
            progressDialog.setMessage(getApplicationContext().getString(R.string.creatingDisk));
            return progressDialog;
        default:
            return null;
        }
    }

    @Override
    protected void onPrepareDialog(int id, Dialog dialog) {
        switch(id) {
        case PROGRESS_DIALOG:
            progressDialog.setProgress(0);
            progressThread = new ProgressThread(handler);
            progressThread.start();
        }
    }

    // Define the Handler that receives messages from the thread and update the progress
    final Handler handler = new Handler() {
        public void handleMessage(Message msg) {
            int total = msg.arg1;
            progressDialog.setProgress(total);
            if (total >= 100){
                dismissDialog(PROGRESS_DIALOG);
                progressThread.setState(ProgressThread.STATE_DONE);
                
                if (msg.arg2 == RESULT_OK)
                {
	    			Intent data = new Intent();
	    			data.putExtra("diskPath", (String)msg.obj);
	    			setResult(RESULT_OK, data);
	    			finish();
                }
                else
                {
                	Toast toast = Toast.makeText(getApplicationContext(), (Integer)(msg.obj), Toast.LENGTH_LONG);
                	toast.show();
                }
            }
        }
    };

    /** Nested class that performs progress calculations (counting) */
    private class ProgressThread extends Thread {
        Handler mHandler;
        final static int STATE_DONE = 0;
        final static int STATE_RUNNING = 1;
        int mState;
        int total;
       
        ProgressThread(Handler h) {
            mHandler = h;
        }
       
        public void run() {
        	
            mState = STATE_RUNNING;
            
        	File dataDir = new File(Environment.getExternalStorageDirectory(), "minivmac");
			String fileName = String.format("%s.dsk", name.getText().toString().replace(" ", "_"));
			File disk = new File(dataDir, fileName);
			try {
				if (!disk.createNewFile())
				{
					// Error show file exist warning
					handleError(null, R.string.errFileExist);
					return;
				}
			} catch (IOException e) {
				e.printStackTrace();
				handleError(null, R.string.errGeneral);
				return;
			}
			
			FileOutputStream writer = null;
			try {
				writer = new FileOutputStream(disk);
			} catch (FileNotFoundException e) {
				e.printStackTrace();
				handleError(disk, R.string.errGeneral);
				return;
			}
			
			int sizeInBytes = size.getProgress() + 400;
			
			byte[] buffer = new byte[1024];
			for (int i = 0 ; i < 1024 ; i++)
				buffer[i] = 0;
			
			try {
				for (int i = 0 ; i < sizeInBytes ; i++)
				{
					writer.write(buffer);
					
					Message msg = mHandler.obtainMessage();
	                msg.arg1 = (int)(99.0 * i / sizeInBytes);
	                mHandler.sendMessage(msg);
				}
				writer.close();
			} catch (IOException e) {
				handleError(disk, R.string.errCreateDisk);
				return;
			}
			
			Message msg = mHandler.obtainMessage();
            msg.arg1 = 100;
            msg.arg2 = RESULT_OK;
            msg.obj = disk.getAbsolutePath();
            mHandler.sendMessage(msg);
        }

		private void handleError(File disk, int error) {
			
			if (disk != null)
			{
				// Delete what we got so far
				disk.delete();
			}
			
			Message msg = mHandler.obtainMessage();
			msg.arg1 = 100;
			msg.arg2 = RESULT_FIRST_USER;
			msg.obj = error;
			mHandler.sendMessage(msg);
		}
        
        /* sets the current state for the thread,
         * used to stop the thread */
        public void setState(int state) {
            mState = state;
        }
    }
}
