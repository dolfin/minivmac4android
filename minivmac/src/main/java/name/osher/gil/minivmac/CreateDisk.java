package name.osher.gil.minivmac;

import android.app.Activity;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.NonNull;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.Toast;

import java.lang.ref.WeakReference;

public class CreateDisk extends Activity {
	private static final int PROGRESS_DIALOG = 0;
    private ProgressThread progressThread;
    private ProgressDialog progressDialog;

	private TextView sizeText;
	private EditText name;
	private SeekBar size;
	private Button create;
	
	@Override
    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);
    	
        setContentView(R.layout.create_disk);
        sizeText = (TextView)findViewById(R.id.sizeText);
        name = (EditText)findViewById(R.id.name);
        size = (SeekBar)findViewById(R.id.size);
        create = (Button)findViewById(R.id.create);
        
        size.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

			public void onStopTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub

			}

			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub

			}

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

			public void onClick(View v) {
				showDialog(PROGRESS_DIALOG);
			}
		});
        
        size.setMax(128000 - 400);
        size.setProgress(20480 - 400);

		Toolbar bar = (Toolbar)findViewById(R.id.toolbar);
		bar.setNavigationOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				finish();
			}
		});
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
    protected void onPrepareDialog(int id, @NonNull Dialog dialog) {
        switch(id) {
        case PROGRESS_DIALOG:
            progressDialog.setProgress(0);
            progressThread = new ProgressThread(new ProgressHandler(this));
            progressThread.start();
        }
    }

    // Define the Handler that receives messages from the thread and update the progress
	private static class ProgressHandler extends Handler {
        private final WeakReference<CreateDisk> mActivityRef;

        ProgressHandler(CreateDisk activity) {
            mActivityRef = new WeakReference<>(activity);
        }

        public void handleMessage(Message msg) {
            final CreateDisk activity = mActivityRef.get();

            int total = msg.arg1;
            if (activity != null) {
                activity.progressDialog.setProgress(total);

                if (total >= 100) {
                    activity.dismissDialog(PROGRESS_DIALOG);
                    activity.progressThread.setState(ProgressThread.STATE_DONE);

                    if (msg.arg2 == RESULT_OK) {
                        Intent data = new Intent();
                        data.putExtra("diskPath", (String) msg.obj);
                        activity.setResult(RESULT_OK, data);
                        activity.finish();
                    } else {
                        Toast toast = Toast.makeText(activity.getApplicationContext(), (Integer) (msg.obj), Toast.LENGTH_LONG);
                        toast.show();
                    }
                }
            }
        }
    }

    /** Nested class that performs progress calculations (counting) */
    private class ProgressThread extends Thread {
        Handler mHandler;
        final static int STATE_DONE = 0;
        final static int STATE_RUNNING = 1;
        int mState;
       
        ProgressThread(Handler h) {
			mHandler = h;
		}
       
        public void run() {
        	
            mState = STATE_RUNNING;

			String fileName = String.format("%s.dsk", name.getText().toString().replace(" ", "_"));
            int sizeInBytes = 1024 * (size.getProgress() + 400);
			FileManager.getInstance().makeNewDisk(sizeInBytes, fileName, FileManager.getInstance().getDataDir().getAbsolutePath(), mHandler);
        }
        
        /* sets the current state for the thread,
         * used to stop the thread */
        public void setState(int state) {
            mState = state;
        }
    }
}
