package name.osher.gil.minivmac;

import static android.widget.AdapterView.INVALID_POSITION;

import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.RadioButton;
import android.widget.TextView;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.text.DecimalFormat;
import java.util.ArrayList;

public class DiskManagerActivity extends AppCompatActivity {
    private static final String TAG = "minivmac.DiskManagerAct";

    private DisksListAdapter _adapter;
	
	@Override
    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);
    	
        setContentView(R.layout.disk_manager);
        ListView list = findViewById(R.id.disksList);
        Button newDisk = findViewById(R.id.newDisk);
        Button importDisk = findViewById(R.id.importDisk);
        Button exportDisk = findViewById(R.id.exportDisk);
        Button remove = findViewById(R.id.remove);

        if (getSupportActionBar() != null) {
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        }

        list.setChoiceMode(AbsListView.CHOICE_MODE_SINGLE);
        _adapter = new DisksListAdapter(this);
        list.setAdapter(_adapter);
        list.setOnItemClickListener((parent, view, position, id) -> _adapter.setSelectedIndex(position));

        newDisk.setOnClickListener(v -> showNewDiskDialog());
        importDisk.setOnClickListener(v -> showOpenFileDialog());
        exportDisk.setOnClickListener(v -> showShareDialog(list.getCheckedItemPosition()));
        remove.setOnClickListener(v -> removeDiskDialog(list.getCheckedItemPosition()));
	}

	@Override
    protected void onResume() {
	    super.onResume();
        refreshDisksList();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) // Press Back Icon
        {
            finish();
        }

        return super.onOptionsItemSelected(item);
    }

    private void refreshDisksList() {
	    if (!FileManager.getInstance().isInitialized()) {
	        FileManager.getInstance().init(this);
        }
        // Initializing a new String Array
        File[] disks = FileManager.getInstance().getAvailableDisks();

        ArrayList<DiskImage> disks_list = new ArrayList<>();

        for (File disk : disks) {
            DiskImage di = new DiskImage(disk, disk.getName());
            disks_list.add(di);
        }

        _adapter.clear();
        _adapter.addAll(disks_list);
    }

    private void showNewDiskDialog() {
        Intent i = new Intent(DiskManagerActivity.this, CreateDiskActivity.class);
        startActivity(i);
    }

    private final ActivityResultLauncher<String> _openFile = registerForActivityResult(new ActivityResultContracts.GetContent(), uri -> {
        if (uri != null) {
            Log.w("DiskManagerActivity", uri.toString());

            InputStream diskFile;
            try {
                diskFile = this.getContentResolver().openInputStream(uri);
            } catch (FileNotFoundException ex) {
                // Unable to open Disk file.
                Log.e(TAG, String.format("Unable to open file: %s", uri), ex);
                return;
            }
            String diskName = FileManager.getInstance().getFileName(uri);
            File dst = FileManager.getInstance().getDisksFile(diskName);
            try {
                FileManager.getInstance().copy(diskFile, dst);
            } catch (IOException ex) {
                // Unable to copy Disk
                Log.e(TAG, String.format("Unable to copy file: %s", uri), ex);
                return;
            }
        } else {
            Log.i(TAG, "No file was selected.");
        }

        refreshDisksList();
    });

	private void showOpenFileDialog() {
        _openFile.launch("application/octet-stream");
    }

    private void showShareDialog(int selectedDiskImage) {
        if (selectedDiskImage != INVALID_POSITION) {
            DiskImage di = _adapter.getItem(selectedDiskImage);
            File file = di.getFile();
            String name = di.getName();

            Utils.showShareDialog(this, file, name);
        } else {
            showNoDiskSelectedDialog();
        }
    }

    private void showNoDiskSelectedDialog() {
        AlertDialog.Builder alert = new AlertDialog.Builder(this);
        alert.setMessage(R.string.noDiskSelected);
        alert.setCancelable(true);
        alert.setNeutralButton(R.string.btn_ok, null);
        AlertDialog d = alert.create();
        d.show();
    }

    private void removeDiskDialog(int selectedDiskImage) {
	    if (selectedDiskImage != INVALID_POSITION) {
            DiskImage di = _adapter.getItem(selectedDiskImage);

            android.app.AlertDialog.Builder alert = new android.app.AlertDialog.Builder(this);
            alert.setMessage(String.format(getString(R.string.removeDiskWarning), di.toString()));
            alert.setCancelable(true);
            alert.setPositiveButton(R.string.btn_yes, (dialog, which) -> removeDisk(di));
            alert.setNegativeButton(R.string.btn_no, null);
            AlertDialog d = alert.create();
            d.show();
        } else {
            showNoDiskSelectedDialog();
        }
    }

    private void removeDisk(DiskImage diskImage) {
        try {
            FileManager.getInstance().delete(diskImage.getFile());
        } catch (Exception ex) {
            android.app.AlertDialog.Builder alert = new android.app.AlertDialog.Builder(this);
            alert.setMessage(String.format(getString(R.string.cantRemoveDisk), diskImage.toString()));
            alert.setCancelable(true);
            alert.setNeutralButton(R.string.btn_ok, null);
            AlertDialog d = alert.create();
            d.show();
        }

        refreshDisksList();
    }

    public class DiskImage {
	    private final File _file;
	    private final String _name;
	    private final long _size;

	    public DiskImage(File file, String name) {
	        _file = file;
	        _name = name;
	        _size = file.length();
        }

        public File getFile() { return _file; }
        public String getName() { return _name; }
        public long getSize() { return _size; }
        public String getReadableSize() {
            DecimalFormat dform = new DecimalFormat("#,###.##");
	        long rs = _size;
	        if (rs < 1024) {
	            return String.format(getString(R.string.sizeInBytes), dform.format(rs));
            } else {
	            rs /= 1024;
	            if (rs < 1024) {
	                return String.format(getString(R.string.sizeInKiB), dform.format(rs));
                } else {
                    rs /= 1024;
                    return String.format(getString(R.string.sizeInMiB), dform.format(rs));
                }
            }
        }

        @NonNull
        @Override
        public String toString() { return getName(); }
    }

    public class DisksListAdapter extends ArrayAdapter<DiskImage> {
        private int _selectedIndex = -1;

        public DisksListAdapter(Context context) {
            super(context, 0);
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            // Get the data item for this position
            DiskImage di = getItem(position);
            // Check if an existing view is being reused, otherwise inflate the view
            if (convertView == null) {
                convertView = LayoutInflater.from(getContext()).inflate(R.layout.simple_list_item_2_single_choice, parent, false);
            }
            // Lookup view for data population
            TextView name = convertView.findViewById(R.id.text1);
            TextView size = convertView.findViewById(R.id.text2);
            RadioButton radioButton = convertView.findViewById(R.id.radio);
            // Populate the data into the template view using the data object
            name.setText(di.getName());
            size.setText(di.getReadableSize());
            radioButton.setChecked(position == _selectedIndex);
            // Return the completed view to render on screen
            return convertView;
        }

        void setSelectedIndex(int index) {
            _selectedIndex = index;
            notifyDataSetChanged();
        }
    }
}
